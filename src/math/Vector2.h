#pragma once

#include "Vector.h"

namespace Stellarium
{

/**
* @brief A class representing a 2D vector.
*/
class Vector2 : public Vector<2>
{
    public:

        /*
        ===================================
             Constructors/Desctructors
        ===================================
        */

        /**
        * @brief Default constructor - initializes to zero vector.
        */
        Vector2() = default;

        /**
        * @brief Constructs a Vector2 object with the given x and y values.
        * @param x The x value of the vector.
        * @param y The y value of the vector.
        */
        Vector2(double x, double y) : Vector<2>({x, y}) { }

        /**
        * @brief Converting constructor from base Vector<2>.
        */
        Vector2(const Vector<2>& v) : Vector<2>(v) { }

};

} // namespace Stellarium
