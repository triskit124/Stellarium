#pragma once

#include "Matrix33.h"

#include <cmath>
#include <math.h>
#include <stdexcept>

namespace Stellarium
{

/**
* @brief A class representing a rotation matrix.
*/
class RotationMatrix: public Matrix33
{
    public:

        /*
        ===================================
             Constructors/Destructors
        ===================================
        */

        /**
        * @brief Default constructor for the RotationMatrix object.
        */
        RotationMatrix() = default;

        /**
        * @brief Constructs a RotationMatrix object with the given x, y, and z vectors.
        * @param x The 1st row of the matrix.
        * @param y The 2nd row of the matrix.
        * @param z The 3rd row of the matrix.
        */
        RotationMatrix(Vector3 x, Vector3 y, Vector3 z) : Matrix33(x, y, z) { 
            validate();
        };

        /**
        * @brief Converting constructor from a Matrix33.
        * @param m The Matrix33 object.
        */
        RotationMatrix(const Matrix33& m) : Matrix33(m) {
            validate();
        }


        /*  
        ====================
               Methods
        ====================
        */

        Vector3 getRollPitchYaw() const {
            // See: https://eecs.qmul.ac.uk/~gslabaugh/publications/euler.pdf

            double roll, pitch, yaw = 0.0;

            if (abs( z[0] - 1) <= _epsilon)
            {
                pitch = -M_PI_2;
                roll = -yaw + atan2(-x[1], -x[2]);
            }
            else if (std::abs( z[0] + 1) <= _epsilon)
            {
                pitch = M_PI_2;
                roll = yaw + atan2( x[1],  x[2]);
            }
            else
            {
                pitch = -asin( z[0]);
                roll = atan2( z[1] / cos(pitch),  z[2] / cos(pitch));
                yaw = atan2( y[0] / cos(pitch),  x[0] / cos(pitch));
            }

            return Vector3(roll, pitch, yaw);
        }

        private:

            void validate() const {
                if ( abs(getDeterminant() - 1.0) > _epsilon ) {
                    throw std::invalid_argument("Rotation matrix must have a determinant of +1. Got: " + std::to_string(getDeterminant()));
                }
                if ( !isOrthogonal() ) {
                    throw std::invalid_argument("Rotation matrix must be orthogonal.");
                }
            }



};



} // end namespace Stellarium
