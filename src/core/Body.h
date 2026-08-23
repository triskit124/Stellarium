#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "Frame.h"
#include "SpatialInertia.h"
#include "SpatialTransform.h"
#include "SpatialVector.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "Joint.h"

namespace Stellarium
{

/**
 * @brief A class representing a rigid body.
 */
class Body
{

    public:

        /*
        ===================================
             Constructors/Desctructors
        ===================================
        */


        /**
        * @brief Constructs a Body object with the given name, mass, center of mass, and inertia.
        * @param name The name of the body.
        * @param mass The mass of the body.
        * @param cm The position of center of mass of the body.
        * @param inertia The inertia matrix of the body.
        */
        Body(const std::string& name, SpatialInertia spatial_inertia, Joint::Info joint_info = { })
        : _name(name), 
        _spatial_inertia(spatial_inertia), 
        _body_frame(name), 
        _center_of_mass_frame(name + "_center_of_mass", spatial_inertia.getCenterOfMass()) 
        {
            attachToParent(joint_info);
        };

        /*
        ==============
            Methods
        ==============
        */

        const std::string getName() const { return _name; };

        /*
        ==========================================================================================
            External forces
        ==========================================================================================
            Forces and torques accumulate until clearForces() is called -- they are NOT reset by a
            physics step, so a constant force only has to be applied once.

            A "force" is taken to act along a line through the BODY FRAME ORIGIN, and a "torque" is
            a pure couple. The body-frame and inertial-frame accumulators are kept apart because
            only the latter has to be rotated into body coordinates each step, which cannot be done
            at the time the force is applied.
        */

        /**
        * @brief Adds a force acting at the body frame origin, expressed in the body frame.
        */
        void addBodyFrameForce(const Vector3& force) { _body_frame_force.setForce(_body_frame_force.getForce() + force); };

        /**
        * @brief Adds a pure torque, expressed in the body frame.
        */
        void addBodyFrameTorque(const Vector3& torque) { _body_frame_force.setTorque(_body_frame_force.getTorque() + torque); };

        /**
        * @brief Adds a force acting at the body frame origin, expressed in the inertial frame.
        */
        void addInertialFrameForce(const Vector3& force) { _inertial_frame_force.setForce(_inertial_frame_force.getForce() + force); };

        /**
        * @brief Adds a pure torque, expressed in the inertial frame.
        */
        void addInertialFrameTorque(const Vector3& torque) { _inertial_frame_force.setTorque(_inertial_frame_force.getTorque() + torque); };

        /**
        * @brief Clears all forces and torques acting on the body.
        */
        void clearForces() { _body_frame_force = SpatialForce(); _inertial_frame_force = SpatialForce(); };

        /**
        * @brief The accumulated external force/torque expressed in the BODY frame.
        */
        SpatialForce getBodyFrameExternalForce() const { return _body_frame_force; };

        /**
        * @brief The accumulated external force/torque expressed in the INERTIAL frame.
        */
        SpatialForce getInertialFrameExternalForce() const { return _inertial_frame_force; };

        /*
        ==========================================================================================
            Pose (written by the simulation's forward-kinematics pass, read by the render layer)
        ==========================================================================================
        */

        Vector3 getPosition() const { return _body_frame.getPosition(); };
        Vector3 getVelocity() const { return _body_frame.getVelocity(); };
        Quaternion getAttitude() const { return _body_frame.getAttitude(); };
        Vector3 getAngularVelocity() const { return _body_frame.getAngularVelocity(); };

        /**
        * @brief Writes this body's pose from its spatial transform and velocity relative to the
        * inertial base, converting into the conventions documented on Frame.
        *
        * @param base_to_body The transform mapping base-frame quantities into this body's frame
        *                     (Featherstone's i_X_0).
        * @param velocity     This body's spatial velocity, expressed in body coordinates.
        */
        void setPoseFromBase(const SpatialTransform& base_to_body, const SpatialVelocity& velocity) {
            // base_to_body holds (E, r): r is the body origin's position in base coordinates, and
            // E maps base components to body components, so the attitude (which maps the other
            // way) is its conjugate. See the convention block in SpatialTransform.h.
            const Quaternion attitude = base_to_body.getRotation().getConjugate();
            const Vector3 position = base_to_body.getTranslation();

            // Frame stores linear velocity in INERTIAL components and angular velocity in FRAME
            // components; the spatial velocity is entirely in body components.
            _body_frame.setPosition(position);
            _body_frame.setAttitude(attitude);
            _body_frame.setVelocity(attitude * velocity.getLinearVelocity());
            _body_frame.setAngularVelocity(velocity.getAngularVelocity());

            // The centre of mass rides along, offset by c in body coordinates. Its velocity picks
            // up the omega x c term since it is a different body-fixed point.
            const Vector3 c = _spatial_inertia.getCenterOfMass();
            _center_of_mass_frame.setPosition(position + attitude * c);
            _center_of_mass_frame.setAttitude(attitude);
            _center_of_mass_frame.setVelocity(attitude * (velocity.getLinearVelocity() + velocity.getAngularVelocity().cross(c)));
            _center_of_mass_frame.setAngularVelocity(velocity.getAngularVelocity());
        }

        Frame& getBodyFrame() { return _body_frame; };
        const Frame& getBodyFrame() const { return _body_frame; };

        Frame& getCenterOfMassFrame() { return _center_of_mass_frame; };
        const Frame& getCenterOfMassFrame() const { return _center_of_mass_frame; };

        Joint* getJoint() { return _joint.get(); };

        SpatialInertia getSpatialInertia() const { return _spatial_inertia; };

        void attachToParent(Joint::Info joint_info) {
            if (joint_info.parent == nullptr) {
                // fixed base/root body (e.g. the world body at the head of a kinematic chain): no joint.
                return;
            }
            if (_joint) {
                throw std::runtime_error("Cannot attach to body " + joint_info.parent->getName() + ". Parent already exists.");
            }
            switch (joint_info.type) {
                case Joint::Type::Pin:
                    _joint = std::make_unique<PinJoint>(this, joint_info);
                    break;
                case Joint::Type::Free:
                    _joint = std::make_unique<FreeJoint>(this, joint_info);
                    break;
                default:
                    throw std::invalid_argument("Joint type is not yet implemented.");
            }

            // register this body as a child with the parent
            // this is mostly for reference purposes. This body owns the joint object.
            joint_info.parent->_addChild(this);
        }

    private:

        const std::string _name;

        SpatialInertia _spatial_inertia;
        SpatialForce _body_frame_force { };
        SpatialForce _inertial_frame_force { };

        Frame _body_frame;
        Frame _center_of_mass_frame;

        std::unique_ptr<Joint> _joint = nullptr;
        std::vector<Body*> _children { };

        void _addChild(Body* new_child) {

            // verify that the new child isn't already a child of this body
            for (Body* child : _children) {
                if (child == new_child) {
                    throw std::runtime_error("Body is already registered as a child of this body.");
                }
            }

            // verify that the new child has a joint that lists this body as a parent
            if (new_child->getJoint()->getInfo().parent != this) {
                throw std::invalid_argument("Tried to call addChild on this body but the child's joint does not list this body as its parent.");
            }

            _children.push_back(new_child);
        }

};

} // end namespace Stellarium
