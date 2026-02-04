#pragma once

#include "Matrix.h"
#include "Vector3.h"

#include <cmath>
#include <stdexcept>

namespace Stellarium
{

/**
* @brief A class representing a 3x3 matrix.
*/
class Matrix33 : public Matrix<3, Vector3>
{
    public:

        /*
        ===================================
             Constructors/Destructors
        ===================================
        */

        /**
        * @brief Default constructor (identity matrix).
        */
        Matrix33() = default;

        /**
        * @brief Default destructor.
        */
        ~Matrix33() = default;

        /**
        * @brief Constructs a Matrix33 object with the given row vectors.
        * @param x The 1st row of the matrix.
        * @param y The 2nd row of the matrix.
        * @param z The 3rd row of the matrix.
        */
        Matrix33(const Vector3& x, const Vector3& y, const Vector3& z)
            : Matrix(std::array<Vector3, 3>{x, y, z}) {}

        /**
        * @brief Converting constructor from base Matrix<3, Vector3>.
        * @param m The base matrix to convert from.
        */
        Matrix33(const Matrix<3, Vector3>& m) : Matrix(m) {}

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
            return _rows[0][0]*(_rows[1][1]*_rows[2][2] - _rows[1][2]*_rows[2][1])
                   - _rows[0][1]*(_rows[1][0]*_rows[2][2] - _rows[1][2]*_rows[2][0])
                   + _rows[0][2]*(_rows[1][0]*_rows[2][1] - _rows[1][1]*_rows[2][0]);
        }

        /**
        * @brief Computes the adjugate of the matrix.
        * @return The adjugate of the matrix.
        */
        Matrix33 getAdjugate() const {
            return Matrix33(
                Vector3(
                    ((_rows[1][1] * _rows[2][2]) - (_rows[2][1] * _rows[1][2])),
                    -((_rows[0][1] * _rows[2][2]) - (_rows[2][1] * _rows[0][2])),
                    ((_rows[0][1] * _rows[1][2]) - (_rows[1][1] * _rows[0][2]))
                ),
                Vector3(
                    -((_rows[1][0] * _rows[2][2]) - (_rows[2][0] * _rows[1][2])),
                    ((_rows[0][0] * _rows[2][2]) - (_rows[2][0] * _rows[0][2])),
                    -((_rows[0][0] * _rows[1][2]) - (_rows[1][0] * _rows[0][2]))
                ),
                Vector3(
                    ((_rows[1][0] * _rows[2][1]) - (_rows[2][0] * _rows[1][1])),
                    -((_rows[0][0] * _rows[2][1]) - (_rows[2][0] * _rows[0][1])),
                    ((_rows[0][0] * _rows[1][1]) - (_rows[1][0] * _rows[0][1]))
                )
            );
        }

        /**
        * @brief Computes the inverse of the matrix.
        * @return The inverse of the matrix.
        */
        Matrix33 getInverse() const {
            // TODO: implement an efficient method that works for any sized matrix
            if (std::abs(getDeterminant()) <= _epsilon)
            {
                throw std::invalid_argument("Matrix is singular and cannot be inverted.");
            }
            return getAdjugate() / getDeterminant();
        }

        /**
        * @brief Checks if the matrix is positive definite.
        * @return True if the matrix is positive definite, false otherwise.
        */
        bool isPositiveDefinite() const {
            double det11 = _rows[0][0];
            double det22 = _rows[0][0]*_rows[1][1] - _rows[0][1]*_rows[1][0];
            double det33 = this->getDeterminant();

            return (det11 > 0) && (det22 > 0) && (det33 > 0);
        }

};

} // namespace Stellarium
