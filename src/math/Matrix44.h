#pragma once

#include <array>

#include "Matrix.h"
#include "Vector4.h"

namespace Stellarium
{

/**
* @brief A class representing a general 4x4 matrix.
*/
class Matrix44 : public Matrix<4, Vector4>
{
    public:

        /**
        * @brief Default constructor - creates identity matrix.
        */
        Matrix44() = default;

        /**
        * @brief Constructs a Matrix44 with the given row vectors.
        * @param row0 The 1st row of the matrix.
        * @param row1 The 2nd row of the matrix.
        * @param row2 The 3rd row of the matrix.
        * @param row3 The 4th row of the matrix.
        */
        Matrix44(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3)
            : Matrix(std::array<Vector4, 4>{row0, row1, row2, row3}) {}

        /**
        * @brief Converting constructor from base Matrix<4, Vector4>.
        * @param m The base matrix to convert from.
        */
        Matrix44(const Matrix<4, Vector4>& m) : Matrix(m) {}

};

} // namespace Stellarium
