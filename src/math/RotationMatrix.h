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
        RotationMatrix() : Matrix33() { };

        /**
        * @brief Constructs a RotationMatrix object with the given x, y, and z vectors. Performs validation to ensure resulting Matrix is a valid RotationMatrix.
        * @param x The 1st row of the matrix.
        * @param y The 2nd row of the matrix.
        * @param z The 3rd row of the matrix.
        * @throws std::invalid_argument if the resulting Matrix is not a valid Rotation matrix (orthogonal with determinant of +1).
        */
        RotationMatrix(const Vector3& x, const Vector3& y, const Vector3& z) : Matrix33(x, y, z) { 
            validate();
        };

        /**
        * @brief Converting constructor from a Matrix33. Performs validation to ensure provided Matrix is a valid RotationMatrix.
        * @throws std::inalid_argument if the Matrix33 is not a valid Rotation matrix (orthogonal with determinant of +1).
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

        /**
        * @brief Returns the Roll-Pitch-Yaw Euler angles of this passive rotation: the frame is rotated by roll about x, then by
        * pitch about the new y, then by yaw about the new z (a body 1-2-3 sequence), i.e. the angles satisfying
        * C = Rz(yaw) Ry(pitch) Rx(roll), where Rx, Ry, Rz are the passive (frame) elementary rotations.
        * Consistent with Quaternion(roll, pitch, yaw) and Quaternion::getRollPitchYaw().
        * @return The Euler angles as (roll, pitch, yaw) in radians.
        */
        Vector3 getRollPitchYaw() const {
            // With (cr, sr), (cp, sp), (cy, sy) the cosines/sines of roll, pitch, yaw, C = Rz(yaw) Ry(pitch) Rx(roll) is
            //
            //     [  cp*cy    cr*sy + sr*sp*cy    sr*sy - cr*sp*cy ]
            //     [ -cp*sy    cr*cy - sr*sp*sy    sr*cy + cr*sp*sy ]
            //     [  sp       -sr*cp               cr*cp           ]
            //
            // At pitch = +/-90 deg (cp = 0) only roll +/- yaw is observable: C[0][1] = sin(roll +/- yaw) and
            // C[1][1] = cos(roll +/- yaw) (+ for pitch = +90, - for pitch = -90), so yaw is set to 0.

            double roll, pitch, yaw = 0.0;

            if (std::abs((*this)[2][0] - 1) <= _epsilon)
            {
                pitch = M_PI_2;
                roll = atan2((*this)[0][1], (*this)[1][1]);
            }
            else if (std::abs((*this)[2][0] + 1) <= _epsilon)
            {
                pitch = -M_PI_2;
                roll = atan2(-(*this)[0][1], (*this)[1][1]);
            }
            else
            {
                pitch = asin((*this)[2][0]);
                roll = atan2(-(*this)[2][1], (*this)[2][2]);
                yaw = atan2(-(*this)[1][0], (*this)[0][0]);
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
