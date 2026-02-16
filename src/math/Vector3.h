#pragma once

#include "Vector.h"

namespace Stellarium
{

/**
* @brief A class representing a 3D vector.
*/
class Vector3 : public Vector
{   
    public:

        /* 
        ===================================
             Constructors/Destructors
        ===================================
        */

        /**
        * @brief Constructs a Vector3 object initialized to the zero vector.
        */
        Vector3() : Vector(3) { }

        /**
        * @brief Constructs a Vector3 object with the given x, y, and z values.
        * @param x The  value of the vector.
        * @param y The y value of the vector.
        * @param z The z value of the vector.
        */
        Vector3(double x, double y, double z) : Vector({x, y, z}) { }

        /**
        * @brief Converting constructor from base Vector
        */
        Vector3(const Vector& v) : Vector(v) {
            if (v.getSize() != 3) {
                throw std::invalid_argument("Cannot convert Vector of size " + std::to_string(v.getSize()) + " to Vector3");
            }
        }

        /**
        * @brief Returns the cross product of the vector with another vector.
        * @param v The vector to cross with.
        * @return The cross product of the two vectors.
        */
        Vector3 cross(const Vector3& v) const {
            return Vector3(
                (*this)[1]*v[2] - (*this)[2]*v[1],
                (*this)[2]*v[0] - (*this)[0]*v[2],
                (*this)[0]*v[1] - (*this)[1]*v[0]
            );
        };

};

} // namespace Stellarium
