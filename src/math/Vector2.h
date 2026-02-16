#pragma once

#include "Vector.h"
#include <vector>

namespace Stellarium
{

/**
* @brief A class representing a 2D vector.
*/
class Vector2 : public Vector
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
        Vector2() : Vector(2) { }

        /**
        * @brief Constructs a Vector2 object with the given x and y values.
        * @param x The x value of the vector.
        * @param y The y value of the vector.
        */
        Vector2(double x, double y) : Vector({x, y}) { }

        /**
        * @brief Converting constructor from base Vector.
        */
        Vector2(const Vector& v) : Vector(v) {
            if (v.getSize() != 2) {
                throw std::invalid_argument("Cannot convert Vector of size " + std::to_string(v.getSize()) + " to Vector2");
            }
        }


};

} // namespace Stellarium
