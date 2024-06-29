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

        /* 
        ===================================
             Constructors/Desctructors 
        ===================================
        */

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

        /* 
        ==============
            Methods
        ==============
        */

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
        * @brief Gets the derivative of the state of the body via rigid-body equations of motion.
        * @return The derivative of the state of the body as an array of doubles.
        */
        std::array<double, 13> getStateDot() const;

        /**
        * @brief Sets the state of the body.
        * @param state The new state of the body as an array of doubles.
        */
        void setState(const std::array<double, 13>& state);

        /**
        * @brief Sets the position state of the body.
        * @param pos The new position of the body.
        */
        void setPosition(const Vector3& pos) { _pos = pos; };
        
        /**
        * @brief Sets the velocity state of the body.
        * @param vel The new velocity of the body.
        */
        void setVelocity(const Vector3& vel) { _vel = vel; };
        
        /**
        * @brief Sets the attitude state of the body.
        * @param att The new attitude of the body.
        */
        void setAttitude(const Quaternion& att) { _att = att; };
        
        /**
        * @brief Sets the angular velocity state of the body.
        * @param ang_vel The new angular velocity of the body.
        */
        void setAngularVelocity(const Vector3& ang_vel) { _ang_vel = ang_vel; };

        /**
        * @brief Gets the position of the body.
        * @return The position of the body.
        */
        Vector3 getPosition() const { return Vector3(_pos); };

        /**
        * @brief Gets the velocity of the body.
        * @return The velocity of the body.
        */
        Vector3 getVelocity() const { return Vector3(_vel); };

        /**
        * @brief Gets the attitude of the body.
        * @return The attitude of the body.
        */
        Quaternion getAttitude() const { return Quaternion(_att); };

        /**
        * @brief Gets the angular velocity of the body.
        * @return The angular velocity of the body.
        */
        Vector3 getAngularVelocity() const { return Vector3(_ang_vel); };

    protected:
        /**
        * @brief The name of the body.
        */
        std::string _name;
        
        /**
        * @brief The mass of the body
        */
        double _mass { 1.0 };
        
        /**
        * @brief The position of the center of mass of the body.
        */
        Vector3 _cm {0.0, 0.0, 0.0};
        
        /**
        * @brief The inertia tensor for the body.
        */
        Matrix33 _inertia = {{1.0, 0.0, 0.0} ,{0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};
        
        /**
        * @brief The position state of the body.
        */
        Vector3 _pos {0.0, 0.0, 0.0};
        
        /**
        * @brief The velocity state of the body.
        */
        Vector3 _vel {0.0, 0.0, 0.0};
        
        /**
        * @brief The acceleration of the body.
        */
        Vector3 _acc {0.0, 0.0, 0.0};
        
        /**
        * @brief The attitude state of the body.
        */
        Quaternion _att {0.0, 0.0, 0.0, 1.0};
        
        /**
        * @brief The angular velocity state of the body.
        */
        Vector3 _ang_vel {0.0, 0.0, 0.0};
        
        /**
        * @brief The angular acceleration of the body.
        */
        Vector3 _ang_acc {0.0, 0.0, 0.0};

        /**
        * @brief The total external force acting on the body.
        */
        Vector3 _force {0.0, 0.0, 0.0};
        
        /**
        * @brief The total external torque acting on the body.
        */
        Vector3 _torque {0.0, 0.0, 0.0};

    private:

};

} // end namespace Stellarium

#endif // end STELL_BODY