#ifndef STELL_BODY
#define STELL_BODY

#include <array>
#include <string>

#include "Math.h"

namespace Stellarium 
{

/**
 * @brief A class representing a rigid body.
 */
class Body
{    

    public:
        /**
        * @brief Constructs a Body object with the given name.
        * @param name The name of the body.
        */
        Body(const std::string& name) : _name(name) {};

        /**
        * @brief Constructs a Body object with the given name, mass, center of mass, and inertia.
        * @param name The name of the body.
        * @param mass The mass of the body.
        * @param cm The position of center of mass of the body.
        * @param inertia The inertia matrix of the body.
        */
        Body(const std::string& name, double mass, const Vector3& cm, const Matrix33& inertia) : _name(name), _mass(mass), _cm(cm), _inertia(inertia) {};

        /**
        * @brief Destructor for the Body object.
        */
        ~Body() {};

        /**
        * @brief Gets the name of the body.
        * @return The name of the body.
        */
        std::string name() const { return _name; };

        /**
        * @brief Sets the name of the body.
        * @param name The new name of the body.
        */
        void name(const std::string& name) { _name = name; };

        /**
        * @brief Gets the mass of the body.
        * @return The mass of the body.
        */
        double mass() const { return _mass; };

        /**
        * @brief Sets the mass of the body.
        * @param mass The new mass of the body.
        */
        void mass(double mass) { _mass = mass; };

        /**
        * @brief Gets the center of mass of the body.
        * @return The center of mass of the body.
        */
        Vector3 cm() const { return _cm; };

        /**
        * @brief Sets the center of mass of the body.
        * @param cm The new center of mass of the body.
        */
        void cm(const Vector3& cm) { _cm = cm; };

        /**
        * @brief Gets the inertia matrix of the body.
        * @return The inertia matrix of the body.
        */
        Matrix33 inertia() const { return _inertia; };

        /**
        * @brief Sets the inertia matrix of the body.
        * @param inertia The new inertia matrix of the body.
        */
        void inertia(const Matrix33& inertia) { _inertia = inertia; };

        /**
        * @brief Adds a force to the body.
        * @param force The force to be added.
        */
        void addForce(const Vector3& force) { _force += force; };

        /**
        * @brief Adds a torque to the body.
        * @param torque The torque to be added.
        */
        void addTorque(const Vector3& torque) { _torque += torque; };

        /**
        * @brief Clears all forces and torques acting on the body.
        */
        void clearForces() { _force = {0.0, 0.0, 0.0}; _torque = {0.0, 0.0, 0.0}; };

        /**
        * @brief Gets the state of the body.
        * @return The state of the body as an array of doubles.
        */
        std::array<double*, 13> getState();

        /**
        * @brief Gets the derivative of the state of the body.
        * @return The derivative of the state of the body as an array of doubles.
        */
        std::array<double, 13> getStateDot() const;

        /**
        * @brief Sets the state of the body.
        * @param state The new state of the body as an array of doubles.
        */
        void setState(const std::array<double, 13>& state);


        void setPosition(const Vector3& pos) { _pos = pos; };
        
        void setVelocity(const Vector3& vel) { _vel = vel; };
        
        void setAttitude(const Quaternion& att) { _att = att; };
        
        void setAngularVelocity(const Vector3& ang_vel) { _ang_vel = ang_vel; };

        Vector3 getPosition() const { return _pos; };

        Vector3 getVelocity() const { return _vel; };

        Quaternion getAttitude() const { return _att; };

        Vector3 getAngularVelocity() const { return _ang_vel; };

    protected:
        std::string _name; /**< The name of the body. */
        double _mass = 1.0; /**< The mass of the body. */
        Vector3 _cm {0.0, 0.0, 0.0}; /**< The center of mass of the body. */
        Matrix33 _inertia = {{1.0, 0.0, 0.0} ,{0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}}; /**< The inertia matrix of the body. */
        Vector3 _pos {0.0, 0.0, 0.0}; /**< The position of the body. */
        Vector3 _vel {0.0, 0.0, 0.0}; /**< The velocity of the body. */
        Vector3 _acc {0.0, 0.0, 0.0}; /**< The acceleration of the body. */
        Quaternion _att {0.0, 0.0, 0.0, 1.0}; /**< The attitude (orientation) of the body. */
        Vector3 _ang_vel {0.0, 0.0, 0.0}; /**< The angular velocity of the body. */
        Vector3 _ang_acc {0.0, 0.0, 0.0}; /**< The angular acceleration of the body. */

        Vector3 _force {0.0, 0.0, 0.0}; /**< The total force acting on the body. */
        Vector3 _torque {0.0, 0.0, 0.0}; /**< The total torque acting on the body. */

    private:

};

} // end namespace Stellarium

#endif // end STELL_BODY