#pragma once

#include "Vector.h"

namespace Stellarium
{

/**
* @brief A class representing a 4D vector.
*/
class Vector4 : public Vector<4>
{
    public:

        /*
        ===================================
             Constructors/Desctructors
        ===================================
        */

        /**
        * @brief Constructs a Vector4 object with the given x, y, z, and w values.
        * @param x The x value of the vector.
        * @param y The y value of the vector.
        * @param z The z value of the vector.
        * @param w The w value of the vector.
        */
        Vector4(double x = 0, double y = 0, double z = 0, double w = 0) : Vector<4>({x, y, z, w}) { }

        /**
        * @brief Converting constructor from base Vector<4>.
        */
        Vector4(const Vector<4>& v) : Vector<4>(v) { }

};

} // namespace Stellarium
