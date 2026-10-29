#pragma once

#include "Matrix.h"
#include "Quaternion.h"
#include "SpatialTransform.h"
#include "SpatialVector.h"
#include "SquareMatrix.h"
#include "Vector.h"
#include "Vector3.h"
#include <cassert>
#include <cstddef>
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
            Body* predecessor = nullptr;          // nullptr means "attach to the simulation's fixed base"
            std::vector<Vector3> axes { };   // joint axes, expressed in the joint frame
            SpatialTransform predecessor_to_joint { }; // predecessor body frame -> joint frame, expressed in predecessor frame (the "XT" of Featherstone table 7.1)
            SpatialTransform successor_to_joint { }; // successor body frame -> joint frame, expressed in successor frame
            Vector q_init { }; // initial value of the joint's position variables
            Vector alpha_init { }; // initial value of the joints velocity variables
        };


        /*
        ===================================
             Constructors/Desctructors
        ===================================
        */

        Joint() = delete;

        Joint(size_t num_dofs, size_t q_size, size_t axes_size, Body* successor, const Info& info) : _q(info.q_init), _alpha(info.alpha_init), _tau(info.alpha_init.getSize(), 0.0), _successor(successor), _info(info) { 
            // Verify sizes are correct
            if (info.axes.size() != axes_size) {
                throw std::invalid_argument("Tried to create joint but info.axes has " + std::to_string(info.axes.size()) + " axes instead of " + std::to_string(axes_size) + ".\n");
            }
            if (info.q_init.getSize() != q_size) {
                throw std::invalid_argument("Tried to create joint but q_init has size " + std::to_string(info.q_init.getSize()) + " instead of " + std::to_string(q_size) + ".\n");
            }
            if (info.alpha_init.getSize() != num_dofs) {
                throw std::invalid_argument("Tried to create joint but info.alpha_init has size " + std::to_string(info.alpha_init.getSize()) + " instead of " + std::to_string(num_dofs) + ".\n");
            }
            
            // Ensure joint axes are normalized
            for (Vector3& joint_axis : _info.axes)
            {
                joint_axis.normalize();
            }

            // Ensure that position variables are normalized
            normalizeConfiguration();
        };

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
        * @brief The motion subspace S expressed in the successor body frame. 
        * Joint implementations define their S in the joint frame,
        * see getJointFrameMotionSubspace(). This method maps S to the 
        * successor frame through successor_to_joint. Featherstone assumes the two 
        * frames coincide, so his S is already body-frame; with a 
        * non-identity successor_to_joint the extra transform is required.
        * Note successor_to_joint is constant, so S is still constant in successor 
        * coordinates and the S_dot * qdot term of c (table 7.1) remains zero.
        */
        Matrix getMotionSubspace() const {
            return _info.successor_to_joint.getInverse().getMotionMatrix() * getJointFrameMotionSubspace();
        }

        // Apparent derivative of motion subspace matrix, ie S_dot. Usually zero except for special cases
        // See: Featherstone, Rigid Body Dynamics Algorithms, 2008, eq. 3.41 (S_dot) 
        Matrix getMotionSubspaceDot() const {
            return _info.successor_to_joint.getInverse().getMotionMatrix() * getJointFrameMotionSubspaceDot();
        }

        /**
        * @brief The motion subspace matrix S expressed in the joint frame, one column per DOF.
        */
        virtual Matrix getJointFrameMotionSubspace() const = 0;

        // transforms from joint frame on predecessor to successor body frame
        virtual SpatialTransform getJointTransform() const = 0;

        /**
        * @brief Apparent time derivative of the motion subspace matrix S expressed in the joint frame, one column per DOF.
        * Usually zero except for special cases.
        */
        virtual Matrix getJointFrameMotionSubspaceDot() const { return Matrix(6, getDegreesOfFreedom()); }

        // Velocity of the successor relative to the predecessor, expressed in successor body frame.
        // See: Featherstone, Rigid Body Dynamics Algorithms, 2008, eq. 3.32 (v_J)
        virtual SpatialMotion getJointVelocity() const { return SpatialMotion(getMotionSubspace() * _alpha) + getBiasVelocity(); }

        // Bias velocity. Usually zero except for special cases.
        // See: Featherstone, Rigid Body Dynamics Algorithms, 2008, eq. 3.32 (sigma(q, t))
        virtual SpatialMotion getBiasVelocity() const { return SpatialMotion(); }

        // Apparent derivative of bias velocity. Usually zero except for special cases.
        // See: Featherstone, Rigid Body Dynamics Algorithms, 2008, eq. 3.41 (sigma_dot(q, t))       
        virtual SpatialMotion getBiasVelocityDot() const { return SpatialMotion(); }

        // Apparent derivative of joint velocity. Usually zero except for special cases.
        // See: Featherstone, Rigid Body Dynamics Algorithms, 2008, eq. 3.41 (c_J) 
        virtual SpatialMotion getJointVelocityDot() const { return SpatialMotion(getMotionSubspaceDot() * _alpha) + getBiasVelocityDot(); }

        /**
        * @brief The time rate of change of the joint position variables, q. Ie: dq/dt.
        * For most joints the velocity coordinates alpha are simply dq/dt; joints with 
        * nq != nv (eg FreeJoint or BallJoint) should override this to
        * appropriately compute dq/dt.
        */
        virtual Vector getQDot() const { return _alpha; }

        /**
        * @brief Re-projects q onto the joint's configuration manifold after an integration step.
        * Only meaningful for redundant parameterizations; a no-op by default.
        */
        virtual void normalizeConfiguration() { }


        const Info& getInfo() const { return _info; }
        Body* getSuccessor() { return _successor; }

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

            // Ensure that _q is normalized
            normalizeConfiguration();
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
        Vector _alpha; // The joint's velocity coordinates. Not d(q)/dt in the general case -- see FreeJoint or BallJoint
        Vector _tau; // The generalized forces acting on the joint

    private:

        Body* _successor;
        Info _info;

};


class SingleDofJoint : public Joint
{
    public:
        SingleDofJoint(Body* successor, const Info& info) : Joint(1, 1, 1, successor, info) { }
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
            // The successor frame is the joint frame rotated by +q about the joint axis, the
            // quaternion class expects an active rotation -- hence the inverse. See the convention block in
            // SpatialTransform.h, and Featherstone's rotx/roty/rotz (eq. 2.24, pp. 23).
            return SpatialTransform(Quaternion(getInfo().axes[0], _q[0]).getInverse(), Vector3());
        }

};

class SliderJoint : public SingleDofJoint
{
    public:
        using SingleDofJoint::SingleDofJoint;
        
        virtual Matrix getJointFrameMotionSubspace() const override {
            Vector3 axis = getInfo().axes[0];
            return Matrix { { 0 }, { 0 }, { 0 }, { axis[0] }, { axis[1] }, { axis[2] } };
        };

        virtual SpatialTransform getJointTransform() const override {
            Vector3 axis = getInfo().axes[0];
            return SpatialTransform(Quaternion(), Vector3(axis[0], axis[1], axis[2]) * _q[0]);
        }
};


/**
* @brief An unconstrained 6-DOF joint, used to give a body a floating base.
*
* The configuration is the redundant 7-parameter pose of the successor frame relative to the joint
* frame on the predecessor, laid out as [qw, qx, qy, qz, x, y, z]:
*   - the quaternion is the successor's attitude in the sense used by Frame, i.e. the active rotation
*     satisfying `v_predecessor = q * v_successor` (so the spatial transform's E is its inverse -- see the
*     convention block in SpatialTransform.h);
*   - the translation is the successor origin's position, expressed in predecessor coordinates.
*
* The velocity coordinates are the successor's spatial velocity expressed in successor coordinates,
* [wx, wy, wz, vx, vy, vz], which makes the motion subspace the 6x6 identity.
*/
class FreeJoint : public Joint
{
    public:

        FreeJoint(Body* successor, const Info& info) : Joint(6, 7, 0, successor, info) { }

        virtual Matrix getJointFrameMotionSubspace() const override { return SquareMatrix(6); };

        virtual SpatialTransform getJointTransform() const override {
            return SpatialTransform(getAttitude().getInverse(), getTranslation());
        }

        virtual Vector getQDot() const override {
            const Quaternion att = getAttitude();
            const Vector3 omega_successor = getAngularVelocity();
            const Vector3 v_successor = getLinearVelocity();

            // Attitude kinematics for an active-rotation attitude quaternion whose angular velocity is expressed
            // in the successor frame: q_dot = 0.5 * q * (0, omega_successor).
            // Should be equivalent to Featherstone, Rigid Body Dynamics Algorithms, 2008, eq. 4.12 except that we use active rotation, not passive.
            const Quaternion att_dot = 0.5 * att * Quaternion(0.0, omega_successor[0], omega_successor[1], omega_successor[2], false);

            // The linear velocity coordinate is the successor origin's velocity in successor coordinates;
            // the translation coordinate lives in predecessor coordinates, so rotate it back.
            const Vector3 r_dot = att * v_successor;

            return Vector { att_dot[0], att_dot[1], att_dot[2], att_dot[3], r_dot[0], r_dot[1], r_dot[2] };
        }

        virtual void normalizeConfiguration() override {
            // calling getAttitude() returns a unit Quaternion representing the attitude
            _q = getAttitude().concatenate(getTranslation());
        }

        /**
        * @brief The successor's attitude relative to the predecessor, i.e. the active rotation satisfying
        * `v_predecessor = getAttitude() * v_successor`. Returns a normalized quaternion.
        */
        Quaternion getAttitude() const { return Quaternion(_q[0], _q[1], _q[2], _q[3]); }

        /**
        * @brief The successor origin's position, expressed in predecessor coordinates.
        * See Featherstone, Rigid Body Dynamics Algorithms, 2008, pp. 81 for discussion of why
        * translation is expressed in predecessor coordinates while velocity is expressed in successor coordinates.
        */
        Vector3 getTranslation() const { return Vector3(_q[4], _q[5], _q[6]); }

        /**
        * @brief The angualr velocity of the successor body w.r.t. predecessor, expressed in successor frame
        */
        Vector3 getAngularVelocity() const { return Vector3(_alpha[0], _alpha[1], _alpha[2]); }

        /**
        * @brief The linear valocity of the successor body w.r.t. predecessor, expressed in successor frame
        */
        Vector3 getLinearVelocity() const { return Vector3(_alpha[3], _alpha[4], _alpha[5]); }
};


class BallJoint : public Joint
{
    public:

        BallJoint(Body* successor, const Info& info) : Joint(3, 4, 0, successor, info) { }

        virtual Matrix getJointFrameMotionSubspace() const override { return SquareMatrix(3).verticalConcatenate(Matrix(3, 3)); };

        virtual SpatialTransform getJointTransform() const override {
            return SpatialTransform(getAttitude().getInverse(), Vector3());
        }

        virtual Vector getQDot() const override {
            const Quaternion att = getAttitude();
            const Vector3 omega_successor = getAngularVelocity();

            // Attitude kinematics for an active-rotation attitude quaternion whose angular velocity is expressed
            // in the successor frame: q_dot = 0.5 * q * (0, omega_successor).
            // Should be equivalent to Featherstone, Rigid Body Dynamics Algorithms, 2008, eq. 4.12 except that we use active rotation, not passive.
            const Quaternion att_dot = 0.5 * att * Quaternion(0.0, omega_successor[0], omega_successor[1], omega_successor[2], false);

            return Vector { att_dot[0], att_dot[1], att_dot[2], att_dot[3] };
        }

        virtual void normalizeConfiguration() override {
            // calling getAttitude() returns a unit Quaternion representing the attitude
            _q = getAttitude();
        }

        /**
        * @brief The successor's attitude relative to the predecessor, i.e. the active rotation satisfying
        * `v_predecessor = getAttitude() * v_successor`. Returns a normalized quaternion.
        */
        Quaternion getAttitude() const { return Quaternion(_q[0], _q[1], _q[2], _q[3]); }

        /**
        * @brief The angualr velocity of the successor body w.r.t. predecessor, expressed in successor frame
        */
        Vector3 getAngularVelocity() const { return Vector3(_alpha[0], _alpha[1], _alpha[2]); }
};


} // end namespace Stellarium
