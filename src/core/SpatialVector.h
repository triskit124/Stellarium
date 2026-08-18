#pragma once

#include "Vector.h"
#include "Vector3.h"
#include <stdexcept>

namespace Stellarium
{

class SpatialForce
{
    public:

        SpatialForce() = default;

        SpatialForce(const Vector3& torque, const Vector3& force) : _torque(torque), _force(force) { };

        Vector3 getTorque() const { return _torque; };

        void setTorque(const Vector3& t) { _torque = t; };

        Vector3 getForce() const { return _force; };

        void setForce(const Vector3& l) { _force = l; };

        Vector getVector() const { return Vector(_torque | _force); };

        SpatialForce operator+(const SpatialForce& f) const {
            return SpatialForce(_torque + f.getTorque(), _force + f.getForce());
        }

        SpatialForce operator-(const SpatialForce& f) const {
            return SpatialForce(_torque - f.getTorque(), _force - f.getForce());
        }


    private:

        Vector3 _torque { };
        Vector3 _force { };
};


class SpatialVelocity
{
    public:

        SpatialVelocity() = default;

        SpatialVelocity(const Vector3& angular_velocity, const Vector3& linear_velocity) : _angular_velocity(angular_velocity), _linear_velocity(linear_velocity) { };

        /**
        * @brief Constructs a SpatialVelocity from a 6-element [angular; linear] vector.
        * @throws std::invalid_argument if vec is not of size 6.
        */
        explicit SpatialVelocity(const Vector& vec) {
            if (vec.getSize() != 6) {
                throw std::invalid_argument("Wrong size to construct SpatialVelocity.");
            }
            _angular_velocity = { vec[0], vec[1], vec[2] };
            _linear_velocity = { vec[3], vec[4], vec[5] };
        }

        Vector3 getAngularVelocity() const { return _angular_velocity; };

        void setAngularVelocity(const Vector3& a) { _angular_velocity = a; };

        Vector3 getLinearVelocity() const { return _linear_velocity; };

        void setLinearVelocity(const Vector3& l) { _linear_velocity = l; };

        Vector getVector() const { return Vector(_angular_velocity | _linear_velocity); };

        SpatialVelocity operator+(const SpatialVelocity& other) const {
            return SpatialVelocity(_angular_velocity + other.getAngularVelocity(), _linear_velocity + other.getLinearVelocity());
        }

        SpatialVelocity operator-(const SpatialVelocity& other) const {
            return SpatialVelocity(_angular_velocity - other.getAngularVelocity(), _linear_velocity - other.getLinearVelocity());
        }

        SpatialVelocity cross(const SpatialVelocity& v) const {
            // See Featherstone, Rigid Body Dynamics Algorithms, 2008, eq. 2.33
            return SpatialVelocity(
                _angular_velocity.cross(v.getAngularVelocity()),
                _angular_velocity.cross(v.getLinearVelocity()) + _linear_velocity.cross(v.getAngularVelocity())
            );
        }

        SpatialForce cross(const SpatialForce& v) const {
            // See Featherstone, Rigid Body Dynamics Algorithms, 2008, eq. 2.34
            return SpatialForce(
                _angular_velocity.cross(v.getTorque()) + _linear_velocity.cross(v.getForce()),
                _angular_velocity.cross(v.getForce())
            );
        }

    private:

        Vector3 _angular_velocity { };
        Vector3 _linear_velocity { };
};






} // namespace Stellarium
