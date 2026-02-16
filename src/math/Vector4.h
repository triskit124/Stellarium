#pragma once

#include "Vector.h"

namespace Stellarium
{

/**
* @brief A class representing a 4D vector.
*/
class Vector4 : public Vector
{
    public:

        /*
        ===================================
             Constructors/Destructors
        ===================================
        */

        /**
        * @brief Default constructor - initializes to zero vector.
        */
        Vector4() : Vector(4) { }

        /**
        * @brief Constructs a Vector4 object with the given x, y, z, and w values.
        * @param x The x value of the vector.
        * @param y The y value of the vector.
        * @param z The z value of the vector.
        * @param w The w value of the vector.
        */
        Vector4(double x, double y, double z, double w) : Vector({x, y, z, w}) { }

        /**
        * @brief Converting constructor from base Vector.
        */
        Vector4(const Vector& v) : Vector(v) {
            if (v.getSize() != 4) {
                throw std::invalid_argument("Cannot convert Vector of size " + std::to_string(v.getSize()) + " to Vector4");
            }
        }

};

} // namespace Stellarium
