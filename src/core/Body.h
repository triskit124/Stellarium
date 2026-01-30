#pragma once

#include <array>
#include <string>

#include "Frame.h"
#include "Vector3.h"
#include "InertiaMatrix.h"
#include "Quaternion.h"

namespace Stellarium
{

/**
 * @brief A class representing a rigid body.
 */
class Body : public Frame
{

    public:

        /**< The size of the state vector for a body. */
        constexpr static unsigned int STATE_SIZE { 13 };

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
        Body(const std::string& name, double mass, const Vector3& cm, const InertiaMatrix& inertia) : Frame(name) {
            this->setMass(mass);
            this->setCm(cm);
            this->setInertia(inertia);
        };

        /*
        ==============
            Methods
        ==============
        */

        /**
        * @brief Gets the mass of the body.
        * @return The mass of the body.
        */
        double getMass() const { return _mass; };

        /**
        * @brief Sets the mass of the body.
        * @param mass The new mass of the body.
        */
        void setMass(double mass) {
            if (mass < 0.0) {
                throw std::invalid_argument("Mass must be a non-negative value");
            }
            _mass = mass;
        };

        /**
        * @brief Gets the center of mass of the body.
        * @return The center of mass of the body.
        */
        Vector3 getCm() const { return _cm; };

        /**
        * @brief Sets the center of mass of the body.
        * @param cm The new center of mass of the body.
        */
        void setCm(const Vector3& cm) { _cm = cm; };

        /**
        * @brief Gets the inertia matrix of the body.
        * @return The inertia matrix of the body.
        */
        InertiaMatrix getInertia() const { return _inertia; };

        /**
        * @brief Sets the inertia matrix of the body.
        * @param inertia The new inertia matrix of the body.
        */
        void setInertia(const InertiaMatrix& inertia) { _inertia = inertia; };

        /**
        * @brief Adds a force to the body.
        * @param force The force to be added, expressed in the body frame.
        */
        void addBodyFrameForce(const Vector3& force) { _force += force; };

        /**
        * @brief Adds a torque to the body.
        * @param torque The torque to be added, expressed in the body frame.
        */
        void addBodyFrameTorque(const Vector3& torque) { _torque += torque; };

        /**
        * @brief Adds a force to the body.
        * @param force The force to be added, expressed in the inertial frame.
        */
        void addInertialFrameForce(const Vector3& force) { _force += _att * force; };

        /**
        * @brief Adds a torque to the body.
        * @param torque The torque to be added, expressed in the inertial frame.
        */
        void addInertialFrameTorque(const Vector3& torque) { _torque += _att * torque; };

        /**
        * @brief Clears all forces and torques acting on the body.
        */
        void clearForces() { _force = {0.0, 0.0, 0.0}; _torque = {0.0, 0.0, 0.0}; };

        /**
        * @brief Gets the state of the body.
        * @return The state of the body as an array of doubles.
        */
        std::array<double*, Body::STATE_SIZE> getState();

        /**
        * @brief Gets the derivative of the state of the body via rigid-body equations of motion.
        * @return The derivative of the state of the body as an array of doubles.
        */
        std::array<double, Body::STATE_SIZE> getStateDot();

    protected:

    private:

        /**
        * @brief The mass of the body
        */
        double _mass { 1.0 };

        /**
        * @brief The location of the center of mass of the body w.r.t the body frame, expressed in the body frame.
        */
        Vector3 _cm {0.0, 0.0, 0.0};

        /**
        * @brief The inertia tensor for the body expressed in the body frame.
        */
        InertiaMatrix _inertia = {{1.0, 0.0, 0.0} ,{0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};


        /**
        * @brief The total external force acting on the body, expressed in the BODY frame.
        */
        Vector3 _force {0.0, 0.0, 0.0};

        /**
        * @brief The total external torque acting on the body, expressed in the BODY frame.
        */
        Vector3 _torque {0.0, 0.0, 0.0};

};

} // end namespace Stellarium
