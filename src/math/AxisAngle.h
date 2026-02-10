#pragma once

#include "Vector3.h"
#include "Quaternion.h"

namespace Stellarium
{

/**
* @brief Axis-angle rotation representation.
*/
class AxisAngle: public MathBase
{
    public:

        /*
        ===================================
             Constructors/Desctructors
        ===================================
        */

        /**
        * @brief Default constructor
        */
        AxisAngle() = default;

        /**
        * @brief Default destructor
        */
        ~AxisAngle() = default;

        /**
        * @brief Constructs an AxisAngle object with the given axis and angle.
        * @param axis The axis of rotation.
        * @param angle The angle of rotation in radians.
        */
        AxisAngle(const Vector3& axis, double angle) {
            this->setAxis(axis);
            this->setAngle(angle);
        };

        /**
        * brief Constructs an AxisAngle object based on a quaternion.
        * @param q The quaternion to convert to an axis-angle representation.
        */
        explicit AxisAngle(const Quaternion& q) {
            Quaternion qq = q.getNormalized();
            _angle = 2 * std::acos(qq[0]);
            double s = std::sqrt(1 - pow(qq[0], 2));
            _axis = Vector3(1, 0, 0);
            if (s > _epsilon)
            {
                _axis = Vector3(qq[1], qq[2], qq[3]) / s;
            }
        };

        /*
        ===================
              Methods
        ===================
        */

        /**
        * @brief Gets the axis of rotation.
        * @return The axis of rotation.
        */
        Vector3 getAxis() const { return _axis; };

        /**
        * @brief Gets the angle of rotation.
        * @return The angle of rotation.
        */
        double getAngle() const { return _angle; };

        /**
        * @brief Sets the axis of rotation.
        * @param axis The new axis of rotation.
        */
        void setAxis(const Vector3& axis) { _axis = axis.getNormalized(); };

        /**
        * @brief Sets the angle of rotation.
        * @param angle The new angle of rotation.
        */
        void setAngle(double angle) { _angle = angle; };

    protected:

    private:
        /**
        * @brief The axis of rotation.
        */
        Vector3 _axis {1.0, 0.0, 0.0};

        /**
        * @brief The angle of rotation in radians.
        */
        double _angle { 0.0 };

};

} // end namespace Stellarium
