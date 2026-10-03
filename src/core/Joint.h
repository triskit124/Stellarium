#pragma once

#include "Matrix.h"
#include "Quaternion.h"
#include "SpatialTransform.h"
#include "SpatialVector.h"
#include "SquareMatrix.h"
#include "Vector.h"
#include "Vector3.h"
#include <stdexcept>
#include <string>
#include <vector>


namespace Stellarium
{

class Body;


/**
 * @brief A class representing an abtract joint.
 */
class Joint
{

    public:

        enum class Type {
            Free,
            Locked,
            Pin,
            Slider,
            Ball
        };

        struct Info {
            Type type = Type::Locked;
            Body* parent = nullptr;          // nullptr means "attach to the simulation's fixed base"
            std::vector<Vector3> axes { };   // joint axes, expressed in the joint frame
            SpatialTransform parent_to_joint { }; // parent body frame -> joint frame, expressed in parent frame (the "XT" of Featherstone table 7.1)
            SpatialTransform child_to_joint { }; // child body frame -> joint frame, expressed in child frame
            Vector q_init { }; // initial value of the joint's position variables
            Vector alpha_init { }; // initial value of the joints velocity variables
        };


        /*
        ===================================
             Constructors/Desctructors
        ===================================
        */


        Joint() = delete;

        /**
        * @brief Constructs a Joint object with the given name, mass, center of mass, and inertia.
        * @param name The name of the Joint.
        * @param mass The mass of the Joint.
        * @param cm The position of center of mass of the Joint.
        * @param inertia The inertia matrix of the Joint.
        */
        Joint(Body* child, const Info& info) : _q(info.q_init), _alpha(info.alpha_init), _tau(info.alpha_init.getSize(), 0.0), _child(child), _info(info)  {};

        virtual ~Joint() = default;

        /*
        ==============
            Methods
        ==============
        */

        /**
        * @brief The number of velocity coordinates (nv) -- the size of alpha, tau, and the number
        * of columns of the motion subspace S. The number of configuration coordinates (nq) is just
        * getQ().getSize(); the two differ only where a redundant parameterization is used (the free
        * joint's unit quaternion: nq = 7, nv = 6).
        */
        size_t getDegreesOfFreedom() const { return _alpha.getSize(); }

        /**
        * @brief The motion subspace S expressed in the child body frame. 
        * Joint implementations define their S in the joint frame,
        * see getJointFrameMotionSubspace(). This method maps S to the 
        * child frame through child_to_joint. Featherstone assumes the two 
        * frames coincide, so his S is already body-frame; with a 
        * non-identity child_to_joint the extra transform is required.
        * Note child_to_joint is constant, so S is still constant in child 
        * coordinates and the S_dot * qdot term of c (table 7.1) remains zero.
        */
        Matrix getMotionSubspace() const {
            return _info.child_to_joint.getInverse().getMotionMatrix() * getJointFrameMotionSubspace();
        }

        /**
        * @brief The motion subspace S expressed in the joint frame, one column per DOF.
        */
        virtual Matrix getJointFrameMotionSubspace() const = 0;

        // transforms from joint frame on parent to child body frame
        virtual SpatialTransform getJointTransform() const = 0;

        // velocity of the successor relative to the predecessor, expressed in child body coordinates.
        // See: Featherstone, Rigid Body Dynamics Algorithms, 2008, eq. 3.33
        virtual SpatialMotion getJointVelocity() const { return SpatialMotion(getMotionSubspace() * _alpha); }

        /**
        * @brief d(q)/dt, of size getQ().getSize(). For most joints the velocity coordinates alpha
        * are simply the derivative of q; joints with nq != nv (see FreeJoint) should override this with the
        * kinematic map that turns velocity coordinates into configuration rates.
        */
        virtual Vector getQDot() const { return _alpha; }

        /**
        * @brief Re-projects q onto the joint's configuration manifold after an integration step.
        * Only meaningful for redundant parameterizations; a no-op by default.
        */
        virtual void normalizeConfiguration() { }


        const Info& getInfo() const { return _info; }
        Body* getChild() { return _child; }

        /**
        * @brief The configuration coordinates q (nq of them).
        */
        const Vector& getQ() const { return _q; };

        /**
        * @brief The velocity coordinates alpha (nv of them). NOT d(q)/dt in general -- see getQDot().
        */
        const Vector& getAlpha() const { return _alpha; };

        void setQ(const Vector& q) {
            if (q.getSize() != _q.getSize()) {
                throw std::invalid_argument("Incorrect size.");
            }
            _q = q;
        }

        void setAlpha(const Vector& alpha) {
            if (alpha.getSize() != _alpha.getSize()) {
                throw std::invalid_argument("Incorrect size.");
            }
            _alpha = alpha;
        }

        // Generalized (joint-space) force/torque applied by an actuator, dual to alpha. Defaults to zero.
        Vector getGeneralizedForce() const { return _tau; };

        void setGeneralizedForce(const Vector& tau) {
            if (tau.getSize() != _tau.getSize()) {
                throw std::invalid_argument("Incorrect size.");
            }
            _tau = tau;
        }


    protected:

        Vector _q; // The joint's position coordinates
        Vector _alpha; // The joint's velocity coordinates. NOT d(q)/dt in the general case -- see FreeJoint
        Vector _tau; // The generalized forces acting on the joint

    private:

        Body* _child;
        Info _info;

};


class SingleDofJoint : public Joint
{
    public:
        SingleDofJoint(Body* child, const Info& info) : Joint(child, info) {
            if (info.axes.size() != 1) {
                throw std::invalid_argument("Tried to create a single dof joint but info.axes has " + std::to_string(info.axes.size()) + " axes instead of 1.");
            }
            if (info.q_init.getSize() != 1 || info.alpha_init.getSize() != 1) {
                throw std::invalid_argument("Tried to create a single dof joint but info.q_init/alpha_init have sizes " + std::to_string(info.q_init.getSize()) + "/" + std::to_string(info.alpha_init.getSize()) + " instead of 1/1.");
            }
        }
};


class PinJoint : public SingleDofJoint
{
    public:
        using SingleDofJoint::SingleDofJoint;
        
        virtual Matrix getJointFrameMotionSubspace() const override {
            Vector3 axis = getInfo().axes[0];
            return Matrix { { axis[0] }, { axis[1] }, { axis[2] }, { 0 }, { 0 }, { 0 } };
        };

        virtual SpatialTransform getJointTransform() const override {
            // The child frame is the joint frame rotated by +q about the joint axis, the
            // quaternion class expects an active rotation -- hence the inverse. See the convention block in
            // SpatialTransform.h, and Featherstone's rotx/roty/rotz (eq. 2.24, pp. 23).
            return SpatialTransform(Quaternion(getInfo().axes[0], _q[0]).getInverse(), Vector3());
        }

};


/**
* @brief An unconstrained 6-DOF joint, used to give a body a floating base.
*
* The configuration is the redundant 7-parameter pose of the child frame relative to the joint
* frame on the parent, laid out as [qw, qx, qy, qz, x, y, z]:
*   - the quaternion is the child's attitude in the sense used by Frame, i.e. the active rotation
*     satisfying `v_parent = q * v_child` (so the spatial transform's E is its inverse -- see the
*     convention block in SpatialTransform.h);
*   - the translation is the child origin's position, expressed in parent coordinates.
*
* The velocity coordinates are the child's spatial velocity expressed in child coordinates,
* [wx, wy, wz, vx, vy, vz], which makes the motion subspace the 6x6 identity.
*/
class FreeJoint : public Joint
{
    public:

        /**
        * @brief The identity configuration: unit quaternion, zero translation.
        */
        static Vector identityConfiguration() { return Vector { 1, 0, 0, 0, 0, 0, 0 }; }

        FreeJoint(Body* child, const Info& info) : Joint(child, info) {
            if (_q.getSize() != 7) {
                throw std::invalid_argument("A free joint needs a q_init of size 7 ([qw,qx,qy,qz,x,y,z]) but got size " + std::to_string(_q.getSize()) + ".");
            }
            if (_alpha.getSize() != 6) {
                throw std::invalid_argument("A free joint needs an alpha_init of size 6 ([wx,wy,wz,vx,vy,vz]) but got size " + std::to_string(_alpha.getSize()) + ".");
            }
            normalizeConfiguration();
        }

        virtual Matrix getJointFrameMotionSubspace() const override { return SquareMatrix(6); };

        virtual SpatialTransform getJointTransform() const override {
            return SpatialTransform(getAttitude().getInverse(), getTranslation());
        }

        virtual Vector getQDot() const override {
            const Quaternion att = getAttitude();
            const Vector3 omega_child = getAngularVelocity();
            const Vector3 v_child = getLinearVelocity();

            // Attitude kinematics for an active-rotation attitude quaternion whose angular velocity is expressed
            // in the child frame: q_dot = 0.5 * q * (0, omega_child).
            // Should be equivalent to Featherstone, Rigid Body Dynamics Algorithms, 2008, eq. 4.12 except that we use active rotation, not passive.
            const Quaternion att_dot = 0.5 * att * Quaternion(0.0, omega_child[0], omega_child[1], omega_child[2], false);

            // The linear velocity coordinate is the child origin's velocity in child coordinates;
            // the translation coordinate lives in parent coordinates, so rotate it back.
            const Vector3 r_dot = att * v_child;

            return Vector { att_dot[0], att_dot[1], att_dot[2], att_dot[3], r_dot[0], r_dot[1], r_dot[2] };
        }

        virtual void normalizeConfiguration() override {
            // calling getAttitude returns a normalized Quaternion representing the attitude
            setQ(getAttitude() | getTranslation());
        }

        /**
        * @brief The child's attitude relative to the parent, i.e. the active rotation satisfying
        * `v_parent = getAttitude() * v_child`. Returns a normalized quaternion.
        */
        Quaternion getAttitude() const { return Quaternion(_q[0], _q[1], _q[2], _q[3]); }

        /**
        * @brief The child origin's position, expressed in parent coordinates.
        * See Featherstone, Rigid Body Dynamics Algorithms, 2008, pp. 81 for discussion of why
        * translation is expressed in parent coordinates while velocity is expressed in child coordinates.
        */
        Vector3 getTranslation() const { return Vector3(_q[4], _q[5], _q[6]); }

        /**
        * @brief The angualr velocity of the child body w.r.t. parent, expressed in child frame
        */
        Vector3 getAngularVelocity() const { return Vector3(_alpha[0], _alpha[1], _alpha[2]); }

        /**
        * @brief The linear valocity of the child body w.r.t. parent, expressed in child frame
        */
        Vector3 getLinearVelocity() const { return Vector3(_alpha[3], _alpha[4], _alpha[5]); }
};


} // end namespace Stellarium
