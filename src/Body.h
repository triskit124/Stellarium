#ifndef STELL_BODY
#define STELL_BODY

#include <array>
#include <string>

#include "Vector3.h"
#include "Matrix33.h"
#include "Quaternion.h"

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
        std::array<double*, 13> getState();

        /**
        * @brief Gets the derivative of the state of the body via rigid-body equations of motion.
        * @return The derivative of the state of the body as an array of doubles.
        */
        std::array<double, 13> getStateDot();

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
    
    private:
        /**
        * @brief The name of the body.
        */
        std::string _name;
        
        /**
        * @brief The mass of the body
        */
        double _mass { 1.0 };
        
        /**
        * @brief The position of the center of mass of the body w.r.t the body frame, expressed in the body frame.
        */
        Vector3 _cm {0.0, 0.0, 0.0};
        
        /**
        * @brief The inertia tensor for the body expressed in the body frame.
        */
        Matrix33 _inertia = {{1.0, 0.0, 0.0} ,{0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};
        
        /**
        * @brief The position of the body w.r.t inertial, expressed in INERTIAL frame.
        */
        Vector3 _pos {0.0, 0.0, 0.0};
        
        /**
        * @brief The velocity of the body w.r.t inertial, expressed in INERTIAL frame.
        */
        Vector3 _vel {0.0, 0.0, 0.0};
        
        /**
        * @brief The acceleration of the body w.r.t inertial, expressed in INERTIAL frame.
        */
        Vector3 _acc {0.0, 0.0, 0.0};
        
        /**
        * @brief The attitude quaternion of the body, representing frame rotation from INERTIAL to BODY frame.
        */
        Quaternion _att {1.0, 0.0, 0.0, 0.0};
        
        /**
        * @brief The angular velocity of the body w.r.t inertial, expressed in the BODY frame.
        */
        Vector3 _ang_vel {0.0, 0.0, 0.0};
        
        /**
        * @brief The angular acceleration of the body w.r.t inertial, expressed in the BODY frame.
        */
        Vector3 _ang_acc {0.0, 0.0, 0.0};

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

#endif // end STELL_BODY