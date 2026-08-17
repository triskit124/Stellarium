#pragma once

#include "Matrix.h"
#include "Quaternion.h"
#include "SpatialTransform.h"
#include "SpatialVector.h"
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
            Type type;
            Body* parent;
            std::vector<Vector3> axes; // in parent frame TODO: check
            SpatialTransform parent_to_joint; // in parent frame
            Vector q_init;
            Vector q_dot_init;
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
        Joint(Body* child, const Info& info) : _child(child), _info(info), _q(info.q_init), _q_dot(info.q_dot_init), _tau(info.q_dot_init.getSize(), 0.0)  {};

        virtual ~Joint() = default;

        /*
        ==============
            Methods
        ==============
        */

        virtual int getDegreesOfFreedom() const = 0;
        virtual Matrix getMotionSubspace() const = 0;

        // transforms from joint frame on parent to child body frame
        virtual SpatialTransform getJointTransform() const = 0;

        // velocity of the successor relative to the predecessor
        virtual SpatialVelocity getJointVelocity() const = 0;


        Info getInfo() const { return _info; }
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


    private:

        Body* _child;
        Info _info;
        Vector _q;
        Vector _q_dot;
        Vector _tau;

};


class SingleDofJoint : public Joint
{
    public:
        SingleDofJoint(Body* child, const Info& info) : Joint(child, info) {
            if (info.axes.size() != 1) {
                throw std::invalid_argument("Tried to create a single dof joint but info.axes has more than one Vector3.");
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
            // TODO: check if inverse on quaternion is needed
            return SpatialTransform(Quaternion(getInfo().axes[0], getQ()[0]), Vector3());
        }

        virtual SpatialVelocity getJointVelocity() const override {
            // See: Featherstone, Rigid Body Dynamics Algorithms, 2008, eq. 3.32
            return SpatialVelocity(getMotionSubspace() * getQDot());
        }

};


} // end namespace Stellarium
