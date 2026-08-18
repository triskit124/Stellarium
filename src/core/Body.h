#pragma once

#include <array>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "Frame.h"
#include "SpatialInertia.h"
#include "SpatialVector.h"
#include "Vector3.h"
#include "InertiaMatrix.h"
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
        Body(const std::string& name, SpatialInertia spatial_inertia, Joint::Info joint_info = { Joint::Type::Free, nullptr, { }, { }, { }, { } })
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

        // /**
        // * @brief Adds a force to the body.
        // * @param force The force to be added, expressed in the body frame.
        // */
        // void addBodyFrameForce(const Vector3& force) { _external_force.setForce(_external_force.getForce() + force); };

        // /**
        // * @brief Adds a torque to the body.
        // * @param torque The torque to be added, expressed in the body frame.
        // */
        // void addBodyFrameTorque(const Vector3& torque) { _external_force.setTorque(_external_force.getTorque() + torque); };

        /**
        * @brief Adds a force to the body.
        * @param force The force to be added, expressed in the inertial frame.
        */
        void addInertialFrameForce(const Vector3& force) { _external_force.setForce(_external_force.getForce() + force); };

        /**
        * @brief Adds a torque to the body.
        * @param torque The torque to be added, expressed in the inertial frame.
        */
        void addInertialFrameTorque(const Vector3& torque) { _external_force.setTorque(_external_force.getTorque() + torque); };

        /**
        * @brief Clears all forces and torques acting on the body.
        */
        void clearForces() { _external_force = SpatialForce(); };

        Frame& getBodyFrame() { return _body_frame; };

        Frame& getCenterOfMassFrame() { return _center_of_mass_frame; };

        Joint* getJoint() { return _joint.get(); };

        SpatialInertia getSpatialInertia() const { return _spatial_inertia; };

        SpatialForce getExternalForce() const { return _external_force; };

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
                default:
                    throw std::invalid_argument("Not yet implemented...");
            }

            // register this body as a child with the parent
            // this is mostly for reference purposes. This body owns the joint object.
            joint_info.parent->_addChild(this);
        }

    private:

        const std::string _name;

        SpatialInertia _spatial_inertia;
        SpatialForce _external_force { };

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
