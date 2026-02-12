#pragma once

#include "Matrix.h"

namespace Stellarium
{

/**
* @brief A class representing an NxN square matrix.
* @tparam N The number of rows/columns of the matrix.
*/
template <size_t N>
class SquareMatrix : public Matrix<N, N>
{
    public:

        /*
        ===================================
             Constructors/Destructors
        ===================================
        */

        /**
        * @brief Default constructor, initializes to identity matrix.
        */
        SquareMatrix() {
            for (size_t i = 0; i < N; ++i) {
                (*this)[i][i] = 1.0;
            }
        };

        /**
        * @brief Constructs a SquareMatrix with the given row vectors.
        * @param rows Array of row vectors.
        */
        SquareMatrix(const std::array<Vector<N>, N>& rows) : Matrix<N, N>(rows) {}

        /**
        * Converting constructor from base Matrix class.
        * @param m The matrix to convert.
        */
        SquareMatrix(const Matrix<N, N>& m) : Matrix<N, N>(m) {}

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
            return (*this * this->getTranspose()) == SquareMatrix();
        }

};

} // namespace Stellarium
