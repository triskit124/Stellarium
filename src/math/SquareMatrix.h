#pragma once

#include "Matrix.h"
#include <cstddef>
#include <initializer_list>

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

};

} // namespace Stellarium
