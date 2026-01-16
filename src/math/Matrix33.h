#pragma once

#include "MathBase.h"
#include "Vector3.h"

#include <cmath>
#include <math.h>
#include <stdexcept>

namespace Stellarium
{

/**
* @brief A class representing a 3x3 matrix.
*/
class Matrix33: public MathBase
{
    public:

        /*
        ===================================
             Constructors/Destructors
        ===================================
        */

        /**
        * @brief Default constructor for the Matrix33 object.
        */
        Matrix33() = default;

        /**
        * @brief Constructs a Matrix33 object with the given x, y, and z vectors.
        * @param x The 1st row of the matrix.
        * @param y The 2nd row of the matrix.
        * @param z The 3rd row of the matrix.
        */
        Matrix33(Vector3 x, Vector3 y, Vector3 z) : x(x), y(y), z(z) {};

        /*
        ====================
            Data Members
        ====================
        */

        /**
        * @brief The 1st row of the matrix.
        */
        Vector3 x {1.0, 0.0, 0.0};

        /**
        * @brief The 2nd row of the matrix.
        */
        Vector3 y {0.0, 1.0, 0.0};

        /**
        * @brief The 3rd row of the matrix.
        */
        Vector3 z {0.0, 0.0, 1.0};

        /*
        ===================
             Operators
        ===================
        */

        /**
        * @brief Operator for left-multiplying a vector by the matrix.
        * @param v The vector to multiply by.
        * @return The vector multiplied by the matrix.
        */
        Vector3 operator*(Vector3 v) const {
            return Vector3(
                x.dot(v),
                y.dot(v),
                z.dot(v)
            );
        };

        /**
        * @brief Operator for element-wise multiplying the matrix by a scalar.
        * @param c The scalar to multiply by.
        * @return The matrix multiplied element-wise by the scalar.
        */
        Matrix33 operator*(double c) const {
            return Matrix33(
                x * c,
                y * c,
                z * c
            );
        };

        /**
        * @brief Operator for element-wise dividing the matrix by a scalar.
        * @param c The scalar to divide by.
        * @return The matrix divided element-wise by the scalar.
        */
        Matrix33 operator/(double c) const {
            return Matrix33(
                x / c,
                y / c,
                z / c
            );
        };

        /**
        * @brief Operator for multiplying this matrix by another matrix.
        * @param m The matrix to left-multiply by the current matrix.
        * @return The resulting matrix.
        */
        Matrix33 operator*(Matrix33 m) const {
            // TODO: check this
            Matrix33 mat = m.getTranspose();
            return Matrix33(
                { x.dot(mat[0]), x.dot(mat[1]), x.dot(mat[2]) },
                { y.dot(mat[0]), y.dot(mat[1]), y.dot(mat[2]) },
                { z.dot(mat[0]), z.dot(mat[1]), z.dot(mat[2]) }
            );
        };

        /**
        * @brief Operator for accessing the 1st, 2nd, and 3rd rows of the matrix.
        * @param idx The index of the row to access.
        * @return The row at the given index.
        */
        Vector3 operator[](int idx) const {
            if (idx == 0)
            {
                return x;
            }
            if (idx == 1)
            {
                return y;
            }
            if (idx == 2)
            {
                return z;
            }
            throw std::invalid_argument("invalid index");
        }

        /**
        * @brief Operator for accessing the 1st, 2nd, and 3rd rows of the matrix.
        * @param idx The index of the row to access.
        * @return The row at the given index.
        */
        Vector3& operator[](int idx) {
            if (idx == 0)
            {
                return x;
            }
            if (idx == 1)
            {
                return y;
            }
            if (idx == 2)
            {
                return z;
            }
            throw std::invalid_argument("invalid index");
        }

        /**
        * @brief Operator testing equality of two matrices.
        * @param m The matrix to compare to.
        * @return True if the matrices are equal, false otherwise.
        */
        bool operator==(const Matrix33& m) const {
            return x == m[0] && y == m[1] && z == m[2];
        };

        /*
        ===================
              Methods
        ===================
        */

        /**
        * @brief Computes the determinant of the matrix.
        * @return The determinant of the matrix.
        */
        double getDeterminant() const {
            return x[0]*(y[1]*z[2] - y[2]*z[1])
                   - x[1]*(y[0]*z[2] - y[2]*z[0])
                   + x[2]*(y[0]*z[1] - y[1]*z[0]);
        };

        /**
        * @brief Computes the transpose of the matrix.
        * @return The transpose of the matrix.
        */
        Matrix33 getTranspose() const {
            return Matrix33(
                Vector3(x[0], y[0], z[0]),
                Vector3(x[1], y[1], z[1]),
                Vector3(x[2], y[2], z[2])
            );
        };

        /**
        * @brief Computes the adjugate of the matrix.
        * @return The adjugate of the matrix.
        */
        Matrix33 getAdjugate() const {
            return Matrix33(
                Vector3(
                    ((y[1] * z[2]) - (z[1] * y[2])),
                    -((x[1] * z[2]) - (z[1] * x[2])),
                    ((x[1] * y[2]) - (y[1] * x[2]))
                ),
                Vector3(
                    -((y[0] * z[2]) - (z[0] * y[2])),
                    ((x[0] * z[2]) - (z[0] * x[2])),
                    -((x[0] * y[2]) - (y[0] * x[2]))
                ),
                Vector3(
                    ((y[0] * z[1]) - (z[0] * y[1])),
                    -((x[0] * z[1]) - (z[0] * x[1])),
                    ((x[0] * y[1]) - (y[0] * x[1]))
                )
            );
        };

        /**
        * @brief Computes the inverse of the matrix.
        * @return The inverse of the matrix.
        */
        Matrix33 inverse() const {
            if (std::abs(getDeterminant()) <= _epsilon)
            {
                throw std::invalid_argument("Matrix is singular and cannot be inverted.");
            }
            return getAdjugate() / getDeterminant();
        };

        /**
        * @brief Prints the matrix to stdout.
        */
        void print() const {
            x.print();
            y.print();
            z.print();
        };

        Vector3 getRollPitchYaw() const {
            // See: https://eecs.qmul.ac.uk/~gslabaugh/publications/euler.pdf
            if (!isRotationMatrix())
            {
                throw std::runtime_error("Cannot get roll-pitch-yaw. Matrix is not a rotation matrix");
            }

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

        bool isRotationMatrix() const {
            return abs(getDeterminant() - 1.0) <= _epsilon && *this * this->getTranspose() == Matrix33();
        }

};

/**
* @brief Global operator for element-wise multiplying a matrix by a scalar on the lefthand side.
* @param c The scalar to multiply by.
* @param m The matrix to multiply by.
* @return The matrix multiplied element-wise by the scalar.
*/
inline Matrix33 operator*(double c, const Matrix33& m)
{
    return m*c;
};

/**
* @brief Global operator for element-wise dividing a matrix by a scalar on the lefthand side.
* @param c The scalar to divide by.
* @param m The matrix to divide.
* @return The matrix divided element-wise by the scalar.
*/
inline Matrix33 operator/(double c, const Matrix33& m)
{
    return m/c;
};


} // end namespace Stellarium
