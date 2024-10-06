#ifndef STELL_FRAME
#define STELL_FRAME

#include "Vector3.h"
#include "Quaternion.h"
#include "Matrix44.h"

namespace Stellarium
{

/**
* @brief A class representing a Frame with a pose.
*/
class Frame
{

    public:
            
        /* 
        ===================================
            Constructors/Destructors 
        ===================================
        */

        /**
        * @brief Constructs a Frame object with the given name, position, and attitude.
        * @param name The name of the frame.
        * @param pos The position of the frame.
        * @param att The attitude of the frame.
        */
        Frame(const std::string& name, const Vector3& pos = Vector3(0, 0, 0), const Quaternion& att = Quaternion(1, 0, 0, 0)) : _name(name), _pos(pos), _att(att) {};        

        /* 
        ===================
              Methods 
        ===================
        */

        /**
        * @brief Gets the name of the frame.
        * @return The name of the frame.
        */
        std::string name() const { return _name; };

        /**
        * @brief Sets the name of the frame.
        * @param name The new name of the frame.
        */
        void name(const std::string& name) { _name = name; };

        /**
        * @brief Sets the position state of the frame.
        * @param pos The new position of the frame.
        */
        void setPosition(const Vector3& pos) { _pos = pos; };
        
        /**
        * @brief Sets the velocity state of the frame.
        * @param vel The new velocity of the frame.
        */
        void setVelocity(const Vector3& vel) { _vel = vel; };
        
        /**
        * @brief Sets the attitude state of the frame.
        * @param att The new attitude of the frame.
        */
        void setAttitude(const Quaternion& att) { _att = att; };
        
        /**
        * @brief Sets the angular velocity state of the frame.
        * @param ang_vel The new angular velocity of the frame.
        */
        void setAngularVelocity(const Vector3& ang_vel) { _ang_vel = ang_vel; };

        /**
        * @brief Gets the position of the frame.
        * @return The position of the frame.
        */
        Vector3 getPosition() const { return Vector3(_pos); };

        /**
        * @brief Gets the velocity of the frame.
        * @return The velocity of the frame.
        */
        Vector3 getVelocity() const { return Vector3(_vel); };

        /**
        * @brief Gets the attitude of the frame.
        * @return The attitude of the frame.
        */
        Quaternion getAttitude() const { return Quaternion(_att); };

        /**
        * @brief Gets the angular velocity of the frame.
        * @return The angular velocity of the frame.
        */
        Vector3 getAngularVelocity() const { return Vector3(_ang_vel); };

        /**
        * @brief Get the pose of the frame with respect to its parent.
        * @return The pose of the frame with respect to its parent.
        */
        Matrix44 getPose() const { return Matrix44(_att, _pos); };

        /**
        * @brief Returns a transformation matrix from this frame to a target frame.
        * @param target The target frame.
        * @return The transformation matrix from this frame to the target frame.
        */
        Matrix44 getTransformTo(const Frame& target) const { return getPose().inverseTransform() * target.getPose(); };

    protected:

        /**
        * @brief The name of the frame.
        */
        std::string _name;

        /**
        * @brief The position of the frame w.r.t inertial, expressed in INERTIAL frame.
        */
        Vector3 _pos {0.0, 0.0, 0.0};
        
        /**
        * @brief The velocity of the frame w.r.t inertial, expressed in INERTIAL frame.
        */
        Vector3 _vel {0.0, 0.0, 0.0};
        
        /**
        * @brief The acceleration of the frame w.r.t inertial, expressed in INERTIAL frame.
        */
        Vector3 _acc {0.0, 0.0, 0.0};
        
        /**
        * @brief The attitude quaternion of the frame, representing active rotation from INERTIAL to FRAME.
        */
        Quaternion _att {1.0, 0.0, 0.0, 0.0};
        
        /**
        * @brief The angular velocity of the frame w.r.t inertial, expressed in this FRAME.
        */
        Vector3 _ang_vel {0.0, 0.0, 0.0};
        
        /**
        * @brief The angular acceleration of the frame w.r.t inertial, expressed in this FRAME.
        */
        Vector3 _ang_acc {0.0, 0.0, 0.0};

};

} // namespace Stellarium

#endif // STELL_FRAME