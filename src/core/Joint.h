#pragma once

#include "Matrix.h"
#include "Quaternion.h"
#include "SpatialTransform.h"
#include "SpatialVector.h"
#include "SquareMatrix.h"
#include "Vector.h"
#include "Vector3.h"
#include <stdexcept>
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
            SpatialTransform parent_to_joint { }; // parent body frame -> joint frame (the "XT" of Featherstone table 7.1)
            Vector q_init { };
            Vector q_dot_init { };
            // TODO: add child_to_joint? Assuming joint is located at child's body frame for now
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
        Joint(Body* child, const Info& info) : _q(info.q_init), _q_dot(info.q_dot_init), _tau(info.q_dot_init.getSize(), 0.0), _child(child), _info(info)  {};

        virtual ~Joint() = default;

        /*
        ==============
            Methods
        ==============
        */

        /**
        * @brief The number of velocity coordinates (nv) -- the size of q_dot, tau, and the number
        * of columns of the motion subspace S.
        */
        virtual int getDegreesOfFreedom() const = 0;

        /**
        * @brief The number of configuration coordinates (nq) -- the size of q. This equals the
        * number of degrees of freedom for every joint whose configuration can be parameterized
        * without singularities, and differs only where a redundant parameterization is used (the
        * free joint's unit quaternion: nq = 7, nv = 6).
        */
        virtual size_t getConfigurationSize() const { return static_cast<size_t>(getDegreesOfFreedom()); }

        virtual Matrix getMotionSubspace() const = 0;

        // transforms from joint frame on parent to child body frame
        virtual SpatialTransform getJointTransform() const = 0;

        // velocity of the successor relative to the predecessor
        virtual SpatialVelocity getJointVelocity() const = 0;

        /**
        * @brief d(q)/dt, of size getConfigurationSize(). For most joints q_dot *is* the derivative
        * of q; joints with nq != nv (see FreeJoint) override this with the kinematic map that
        * turns velocity coordinates into configuration rates.
        */
        virtual Vector getConfigurationDerivative() const { return _q_dot; }

        /**
        * @brief Re-projects q onto the joint's configuration manifold after an integration step.
        * Only meaningful for redundant parameterizations; a no-op by default.
        */
        virtual void normalizeConfiguration() { }


        const Info& getInfo() const { return _info; }
        Body* getChild() { return _child; }

        Vector getQ() const { return _q; };
        Vector getQDot() const { return _q_dot; };

        void setQ(const Vector& q) {
            if (q.getSize() != _q.getSize()) {
                throw std::invalid_argument("Incorrect size.");
            }
            _q = q;
        }

        void setQDot(const Vector& q_dot) {
            if (q_dot.getSize() != _q_dot.getSize()) {
                throw std::invalid_argument("Incorrect size.");
            }
            _q_dot = q_dot;
        }

        // Generalized (joint-space) force/torque applied by an actuator, dual to q_dot. Defaults to zero.
        Vector getGeneralizedForce() const { return _tau; };

        void setGeneralizedForce(const Vector& tau) {
            if (tau.getSize() != _tau.getSize()) {
                throw std::invalid_argument("Incorrect size.");
            }
            _tau = tau;
        }


    protected:

        Vector _q;
        Vector _q_dot;
        Vector _tau;

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
            if (info.q_init.getSize() != 1 || info.q_dot_init.getSize() != 1) {
                throw std::invalid_argument("Tried to create a single dof joint but info.q_init/q_dot_init have sizes " + std::to_string(info.q_init.getSize()) + "/" + std::to_string(info.q_dot_init.getSize()) + " instead of 1/1.");
            }
        }

        virtual int getDegreesOfFreedom() const override { return 1; };
};


class PinJoint : public SingleDofJoint
{
    public:
        using SingleDofJoint::SingleDofJoint;
        
        virtual Matrix getMotionSubspace() const override {
            // S is 6x(dof) -- one column per joint DOF, mapping qdot to a spatial velocity.
            Vector3 axis = getInfo().axes[0];
            return Matrix { { axis[0] }, { axis[1] }, { axis[2] }, { 0 }, { 0 }, { 0 } };
        };

        virtual SpatialTransform getJointTransform() const override {
            // The child frame is the joint frame rotated by +q about the joint axis, so the
            // *coordinate* transform from joint frame to child frame is the transpose of that
            // active rotation -- hence the conjugate. See the convention block in
            // SpatialTransform.h, and Featherstone's rotx/roty/rotz (eq. 2.24, pp. 23).
            return SpatialTransform(Quaternion(getInfo().axes[0], _q[0]).getConjugate(), Vector3());
        }

        virtual SpatialVelocity getJointVelocity() const override {
            // See: Featherstone, Rigid Body Dynamics Algorithms, 2008, eq. 3.32
            return SpatialVelocity(getMotionSubspace() * _q_dot);
        }

};


/**
* @brief An unconstrained 6-DOF joint, used to give a body a floating base.
*
* The configuration is the redundant 7-parameter pose of the child frame relative to the joint
* frame on the parent, laid out as [qw, qx, qy, qz, x, y, z]:
*   - the quaternion is the child's attitude in the sense used by Frame, i.e. the active rotation
*     satisfying `v_parent = q * v_child` (so the spatial transform's E is its conjugate -- see the
*     convention block in SpatialTransform.h);
*   - the translation is the child origin's position, expressed in parent coordinates.
*
* The velocity coordinates are the child's spatial velocity expressed in CHILD coordinates,
* [wx, wy, wz, vx, vy, vz], which makes the motion subspace the 6x6 identity.
*/
class FreeJoint : public Joint
{
    public:

        static constexpr size_t CONFIGURATION_SIZE = 7;
        static constexpr size_t NUM_DOF = 6;

        /**
        * @brief The identity configuration: unit quaternion, zero translation.
        */
        static Vector identityConfiguration() { return Vector { 1, 0, 0, 0, 0, 0, 0 }; }

        FreeJoint(Body* child, const Info& info) : Joint(child, info) {
            if (_q.getSize() != CONFIGURATION_SIZE) {
                throw std::invalid_argument("A free joint needs a q_init of size 7 ([qw,qx,qy,qz,x,y,z]) but got size " + std::to_string(_q.getSize()) + ".");
            }
            if (_q_dot.getSize() != NUM_DOF) {
                throw std::invalid_argument("A free joint needs a q_dot_init of size 6 ([wx,wy,wz,vx,vy,vz]) but got size " + std::to_string(_q_dot.getSize()) + ".");
            }
            normalizeConfiguration();
        }

        virtual int getDegreesOfFreedom() const override { return static_cast<int>(NUM_DOF); };

        virtual size_t getConfigurationSize() const override { return CONFIGURATION_SIZE; };

        virtual Matrix getMotionSubspace() const override { return SquareMatrix(NUM_DOF); };

        virtual SpatialTransform getJointTransform() const override {
            return SpatialTransform(getAttitude().getConjugate(), getTranslation());
        }

        virtual SpatialVelocity getJointVelocity() const override {
            // S is the identity, so the velocity coordinates *are* the joint's spatial velocity.
            return SpatialVelocity(_q_dot);
        }

        virtual Vector getConfigurationDerivative() const override {
            const Quaternion att = getAttitude();
            const Vector3 omega_child = SpatialVelocity(_q_dot).getAngularVelocity();
            const Vector3 v_child = SpatialVelocity(_q_dot).getLinearVelocity();

            // Attitude kinematics for an attitude quaternion whose angular velocity is expressed
            // in the CHILD frame: q_dot = 0.5 * q * (0, omega_child).
            const Quaternion att_dot = 0.5 * att * Quaternion(0.0, omega_child[0], omega_child[1], omega_child[2], false);

            // The linear velocity coordinate is the child origin's velocity in child coordinates;
            // the translation coordinate lives in parent coordinates, so rotate it back.
            const Vector3 r_dot = att * v_child;

            return Vector { att_dot[0], att_dot[1], att_dot[2], att_dot[3], r_dot[0], r_dot[1], r_dot[2] };
        }

        virtual void normalizeConfiguration() override {
            Quaternion att = getAttitude().getNormalized();
            for (size_t i = 0; i < 4; ++i) {
                _q[i] = att[i];
            }
        }

        /**
        * @brief The child's attitude relative to the parent, i.e. the active rotation satisfying
        * `v_parent = getAttitude() * v_child`.
        */
        Quaternion getAttitude() const { return Quaternion(_q[0], _q[1], _q[2], _q[3]); }

        /**
        * @brief The child origin's position, expressed in parent coordinates.
        */
        Vector3 getTranslation() const { return Vector3(_q[4], _q[5], _q[6]); }
};


} // end namespace Stellarium
