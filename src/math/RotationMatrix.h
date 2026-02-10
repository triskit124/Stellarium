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
        RotationMatrix(const Vector3& x, const Vector3& y, const Vector3& z) : Matrix33(x, y, z) { 
            validate();
        };

        /**
        * @brief Converting constructor from a Matrix33.
        * @param m The Matrix33 object.
        */
        explicit RotationMatrix(const Matrix33& m) : Matrix33(m) {
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

            if (abs((*this)[2][0] - 1) <= _epsilon)
            {
                pitch = -M_PI_2;
                roll = -yaw + atan2(-(*this)[0][1], -(*this)[0][2]);
            }
            else if (std::abs((*this)[2][0] + 1) <= _epsilon)
            {
                pitch = M_PI_2;
                roll = yaw + atan2((*this)[0][1], (*this)[0][2]);
            }
            else
            {
                pitch = -asin((*this)[2][0]);
                roll = atan2((*this)[2][1] / cos(pitch), (*this)[2][2] / cos(pitch));
                yaw = atan2((*this)[1][0] / cos(pitch), (*this)[0][0] / cos(pitch));
            }

            return Vector3(roll, pitch, yaw);
        }

        private:

            void validate() const {
                if (abs(getDeterminant() - 1.0) > _epsilon ) {
                    throw std::invalid_argument("Rotation matrix must have a determinant of +1. Got: " + std::to_string(getDeterminant()));
                }
                if (!isOrthogonal()) {
                    throw std::invalid_argument("Rotation matrix must be orthogonal.");
                }
            }



};



} // end namespace Stellarium
