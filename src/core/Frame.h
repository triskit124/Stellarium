#pragma once

#include "Quaternion.h"
#include "SpatialVector.h"
#include "SpatialTransform.h"
#include "Vector3.h"

namespace Stellarium
{

/**
* @brief A class representing a Frame.
*/
class Frame
{

    public:

        /*
        ===================================
            Constructors/Destructors
        ===================================
        */

        Frame() = delete;
        virtual ~Frame() = default;

        /**
        * @brief Constructs a Frame object with the given name, position, and attitude.
        * @param name The name of the frame.
        * @param pose The pose of the frame w.r.t. the parent frame.
        * @param parent The parent frame.
        */
        explicit Frame(const std::string& name, const SpatialTransform& pose = SpatialTransform()) : _name(name), _pose(pose) {};

        /*
        ===================
              Methods
        ===================
        */

        /**
        * @brief Gets the name of the frame.
        * @return The name of the frame.
        */
        std::string getName() const { return _name; };

        /**
        * @brief Sets the pose of the frame w.r.t. inertial.
        * @param pose The new pose of the frame.
        */
        void setPose(const SpatialTransform& pose) { _pose = pose; };

        /**
        * @brief Sets only the position-portion of the pose of the frame w.r.t. inertial.
        * @param pos The new position of the frame.
        */
        void setPosition(const Vector3& pos) { setPose(SpatialTransform(getPose().getRotation(), pos)); }

        /**
        * @brief Sets only the attitude-portion of the pose of the frame w.r.t. inertial.
        * @param pos The new position of the frame.
        */
        void setAttitude(const Quaternion& att) { setPose(SpatialTransform(att, getPose().getTranslation())); }

        /**
        * @brief Sets the velocity of the frame w.r.t. inertial
        * The angular portion (w) is expressed in this frame.
        * The linear portion (v) is expressed in the inertial frame.
        * @param vel The new velocity of the frame.
        */
        void setVelocity(const SpatialMotion& velocity) { _velocity = velocity; };

        /**
        * @brief Sets the acceleration of the frame w.r.t. inertial
        * @param vel The new velocity of the frame.
        */
        void setAcceleration(const SpatialMotion& acceleration) { _acceleration = acceleration; };

        /**
        * @brief Get the Spatial Transform (q, r) of this frame with respect to the inertial frame.
        * The attitude portion (q) is a quaternion that encodes the passive rotation from the inertial frame to this frame.
        * The linear portion (r) is the location of this frame's origin w.r.t. inetial, expressed in the inertial frame.
        * @return The pose of the frame w.r.t. the inertial frame.
        */
        SpatialTransform getPose() const { return _pose; };

        /**
        * @brief Gets the spatial velocity of the frame w.r.t. inertial
        * The angular portion (w) is expressed in this frame.
        * The linear portion (v) is expressed in the inertial frame.
        * @return The velocity of the frame.
        */
        SpatialMotion getVelocity() const { return _velocity; };

        /**
        * @brief Gets the spatial acceleration of the frame w.r.t. inertial
        * @return The velocity of the frame.
        */
        SpatialMotion getAcceleration() const { return _acceleration; };

        /**
        * @brief Returns the spatial transform target_X_this, mapping quantities expressed in this frame into the
        * target frame. Its translation is the target's origin expressed in this frame, and its rotation is the passive
        * rotation from this frame to the target.
        * @param target The target frame.
        * @return The transform from this frame to the target frame.
        */
        SpatialTransform getTransformTo(const Frame& target) const { return target.getPose() * getPose().getInverse(); };

    private:

        /**
        * @brief The name of the frame.
        */
        std::string _name;

        /**
        * @brief The Spatial Transform (q, r) of this frame with respect to the inertial frame.
        * The attitude portion (q) is a quaternion that encodes the passive rotation from the inertial frame to this frame.
        * The linear portion (r) is the location of this frame's origin w.r.t. inetial, expressed in the inertial frame.
        */
        SpatialTransform _pose { };

        /** 
        * @brief The Spatial Velocity (w, v) of this frame w.r.t. inertial.
        * The angular portion (w) is expressed in this frame.
        * The linear portion (v) is expressed in the inertial frame.
        */
        SpatialMotion _velocity { };

        /** 
        * @brief The Spatial Acceleration (w_dot, v_dot) of this frame w.r.t. inertial.
        * The angular portion (w_dot) is expressed in this frame.
        * The linear portion (v_dot) is expressed in the inertial frame.
        */
        SpatialMotion _acceleration { };
};

} // namespace Stellarium
