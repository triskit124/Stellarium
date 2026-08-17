#pragma once

#include "Vector.h"
#include "Vector3.h"

namespace Stellarium
{


class SpatialVelocity
{
    public:

        SpatialVelocity() = default;

        SpatialVelocity(const Vector3& angular_velocity, const Vector3& linear_velocity) : _angular_velocity(angular_velocity), _linear_velocity(linear_velocity) { };

        Vector3 getAngularVelocity() const { return _angular_velocity; };

        void setAngularVelocity(const Vector3& a) { _angular_velocity = a; };

        Vector3 getLinearVelocity() const { return _linear_velocity; };

        void setLinearVelocity(const Vector3& l) { _linear_velocity = l; };

        Vector getVector() const { return Vector(_angular_velocity | _linear_velocity); };



    private:

        Vector3 _angular_velocity { };
        Vector3 _linear_velocity { };
};


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


    private:

        Vector3 _torque { };
        Vector3 _force { };
};



} // namespace Stellarium
