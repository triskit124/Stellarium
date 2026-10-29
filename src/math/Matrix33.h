#pragma once

#include "SquareMatrix.h"
#include "Vector.h"
#include "Vector3.h"

#include <cmath>
#include <stdexcept>

namespace Stellarium
{

/**
* @brief A class representing a 3x3 matrix.
*/
class Matrix33 : public SquareMatrix
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
        Matrix33() : SquareMatrix(3) { };

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
            : SquareMatrix({x, y, z}) {}

        /**
        * @brief Converting constructor from base SquareMatrix
        * @param m The base matrix to convert from.
        */
        Matrix33(const SquareMatrix& m) : SquareMatrix(m) {
            if (m.getNumRows() != 3 || m.getNumCols() != 3) {
                throw std::invalid_argument("Cannot convert non-3x3 SquareMatrix to Matrix33. Matrix has " + std::to_string(m.getNumRows()) + " rows and " + std::to_string(m.getNumCols()) + " columns.");
            }
        }

        /**
        * @brief Converting constructor from base Matrix
        * @param m The base matrix to convert from.
        */
        Matrix33(const Matrix& m) : SquareMatrix(m) { 
            if (m.getNumRows() != 3 || m.getNumCols() != 3) {
                throw std::invalid_argument("Cannot convert non-3x3 Matrix to Matrix33. Matrix has " + std::to_string(m.getNumRows()) + " rows and " + std::to_string(m.getNumCols()) + " columns.");
            }
        }

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
            return (*this)[0][0]*((*this)[1][1]*(*this)[2][2] - (*this)[1][2]*(*this)[2][1])
                   - (*this)[0][1]*((*this)[1][0]*(*this)[2][2] - (*this)[1][2]*(*this)[2][0])
                   + (*this)[0][2]*((*this)[1][0]*(*this)[2][1] - (*this)[1][1]*(*this)[2][0]);
        }

        /**
        * @brief Computes the adjugate of the matrix.
        * @return The adjugate of the matrix.
        */
        Matrix33 getAdjugate() const {
            return Matrix33(
                Vector3(
                    (((*this)[1][1] * (*this)[2][2]) - ((*this)[2][1] * (*this)[1][2])),
                    -(((*this)[0][1] * (*this)[2][2]) - ((*this)[2][1] * (*this)[0][2])),
                    (((*this)[0][1] * (*this)[1][2]) - ((*this)[1][1] * (*this)[0][2]))
                ),
                Vector3(
                    -(((*this)[1][0] * (*this)[2][2]) - ((*this)[2][0] * (*this)[1][2])),
                    (((*this)[0][0] * (*this)[2][2]) - ((*this)[2][0] * (*this)[0][2])),
                    -(((*this)[0][0] * (*this)[1][2]) - ((*this)[1][0] * (*this)[0][2]))
                ),
                Vector3(
                    (((*this)[1][0] * (*this)[2][1]) - ((*this)[2][0] * (*this)[1][1])),
                    -(((*this)[0][0] * (*this)[2][1]) - ((*this)[2][0] * (*this)[0][1])),
                    (((*this)[0][0] * (*this)[1][1]) - ((*this)[1][0] * (*this)[0][1]))
                )
            );
        }

        /**
        * @brief Computes the inverse of the matrix.
        * @return The inverse of the matrix.
        */
        Matrix33 getInverse() const {
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
            double det11 = (*this)[0][0];
            double det22 = (*this)[0][0]*(*this)[1][1] - (*this)[0][1]*(*this)[1][0];
            double det33 = this->getDeterminant();

            return (det11 > 0) && (det22 > 0) && (det33 > 0);
        }

        /**
        * @brief Constructs the skew-symmetric (cross-product operator) matrix for a vector,
        *        such that Matrix33::skew(v) * u == v.cross(u).
        * @param v The vector to construct the skew-symmetric matrix from.
        * @return The skew-symmetric matrix representation of v.
        */
        static Matrix33 skew(const Vector3& v) {
            return Matrix33(
                Vector3(0, -v[2], v[1]),
                Vector3(v[2], 0, -v[0]),
                Vector3(-v[1], v[0], 0)
            );
        }

};

} // namespace Stellarium
