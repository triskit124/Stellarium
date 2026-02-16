#pragma once

#include "SquareMatrix.h"
#include "Vector4.h"

namespace Stellarium
{

/**
* @brief A class representing a general 4x4 matrix.
*/
class Matrix44 : public SquareMatrix
{
    public:

        /**
        * @brief Default constructor - creates identity matrix.
        */
        Matrix44() : SquareMatrix(4) { };

        /**
        * @brief Constructs a Matrix44 with the given row vectors.
        * @param row0 The 1st row of the matrix.
        * @param row1 The 2nd row of the matrix.
        * @param row2 The 3rd row of the matrix
        * @param row3 The 4th row of the matrix.
        */
        Matrix44(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3)
            : SquareMatrix({row0, row1, row2, row3}) {}

        /**
        * @brief Converting constructor from base SquareMatrix.
        * @param m The base matrix to convert from.
        */
        Matrix44(const SquareMatrix& m) : SquareMatrix(m) {
            if (m.getNumRows() != 4 || m.getNumCols() != 4) {
                throw std::invalid_argument("Cannot convert non-4x4 SquareMatrix to Matrix44. Matrix has " + std::to_string(m.getNumRows()) + " rows and " + std::to_string(m.getNumCols()) + " columns.");
            }
        }

        /**
        * @brief Converting constructor from base Matrix.
        * @param m The base matrix to convert from.
        */
        Matrix44(const Matrix& m) : SquareMatrix(m) {
            if (m.getNumRows() != 4 || m.getNumCols() != 4) {
                throw std::invalid_argument("Cannot convert non-4x4 Matrix to Matrix44. Matrix has " + std::to_string(m.getNumRows()) + " rows and " + std::to_string(m.getNumCols()) + " columns.");
            }
        }

};

} // namespace Stellarium
