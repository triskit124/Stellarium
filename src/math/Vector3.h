#pragma once

#include "Vector.h"

namespace Stellarium
{

/**
* @brief A class representing a 3D vector.
*/
class Vector3 : public Vector<3>
{   
    public:

        /* 
        ===================================
             Constructors/Desctructors 
        ===================================
        */

        /**
        * @brief Constructs a Vector3 object with the given x, y, and z values.
        * @param x The x value of the vector.
        * @param y The y value of the vector.
        * @param z The z value of the vector.
        */
        Vector3(double x = 0, double y = 0, double z = 0) : Vector<3>({x, y, z}) { }

        /**
        * @brief Converting constructor from base Vector<3>.
        */
        Vector3(const Vector<3>& v) : Vector<3>(v) { }

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
