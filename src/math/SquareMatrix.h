#pragma once

#include "Matrix.h"
#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <utility>

namespace Stellarium
{

/**
* @brief A class representing an NxN square matrix.
*/
class SquareMatrix : public Matrix
{
    public:

        /*
        ===================================
             Constructors/Destructors
        ===================================
        */

        /**
        * @brief Constructs an identity SquareMatrix of the given size.
        * @param size The number of rows and columns in the matrix.
        */
        SquareMatrix(size_t size) : Matrix(size, size) {
            for (size_t i = 0; i < getNumRows(); ++i) {
                (*this)[i][i] = 1.0;
            }
        };

        /**
        * @brief Constructs a SquareMatrix with the given row vectors.
        * @param rows Initializer list of row vectors.
        */
        SquareMatrix(const std::initializer_list<Vector>& rows) : Matrix(rows) {
            if (getNumRows() != getNumCols()) {
                throw std::invalid_argument("Cannot construct SquareMatrix with non-square row vectors. Matrix has " + std::to_string(getNumRows()) + " rows and " + std::to_string(getNumCols()) + " columns.");
            }
        }

        /**
        * Converting constructor from base Matrix class.
        * @param m The matrix to convert.
        * @throws std::invalid_argument if m is not square (num rows != num cols)
        */
        SquareMatrix(const Matrix& m) : Matrix(m) {
            if (m.getNumRows() != m.getNumCols()) {
                throw std::invalid_argument("Cannot convert non-square matrix to SquareMatrix. Matrix has " + std::to_string(m.getNumRows()) + " rows and " + std::to_string(m.getNumCols()) + " columns.");
            }
        }

        /**
        * @brief Default destructor.
        */
        virtual ~SquareMatrix() = default;

  
        /*
        ===================
              Methods
        ===================
        */

        /**
        * @brief Checks if the matrix is symmetric.
        * @return True if the matrix is symmetric, false otherwise.
        */
        bool isSymmetric() const {
            return *this == this->getTranspose();
        }

        /**
        * @brief Checks if the matrix is orthogonal.
        * @return True if the matrix is orthogonal, false otherwise.
        */
        bool isOrthogonal() const {
            return (*this * this->getTranspose()) == SquareMatrix(getNumRows());
        }

        /**
        * @brief Computes the inverse of the matrix via Gauss-Jordan elimination with partial pivoting.
        * @throws std::invalid_argument if the matrix is singular.
        * @return The inverse of the matrix.
        */
        SquareMatrix getInverse() const {
            const size_t n = getNumRows();
            Matrix work = *this;
            SquareMatrix result(n); // identity

            for (size_t col = 0; col < n; ++col) {
                // partial pivot: find the largest-magnitude entry in this column at or below the diagonal
                size_t pivot_row = col;
                double pivot_val = std::abs(work[col][col]);
                for (size_t row = col + 1; row < n; ++row) {
                    if (std::abs(work[row][col]) > pivot_val) {
                        pivot_row = row;
                        pivot_val = std::abs(work[row][col]);
                    }
                }

                if (pivot_val <= _epsilon) {
                    throw std::invalid_argument("Matrix is singular and cannot be inverted.");
                }

                if (pivot_row != col) {
                    std::swap(work[col], work[pivot_row]);
                    std::swap(result[col], result[pivot_row]);
                }

                double pivot = work[col][col];
                work[col] = work[col] / pivot;
                result[col] = result[col] / pivot;

                for (size_t row = 0; row < n; ++row) {
                    if (row == col) {
                        continue;
                    }
                    double factor = work[row][col];
                    if (std::abs(factor) <= _epsilon) {
                        continue;
                    }
                    work[row] = work[row] - work[col] * factor;
                    result[row] = result[row] - result[col] * factor;
                }
            }

            return result;
        }

};

} // namespace Stellarium
