#pragma once

#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <vector>

#include "MathBase.h"
#include "Vector.h"

namespace Stellarium
{

/**
* @brief A class representing an MxN matrix. Size must be given at construction time. Cannot be resized after construction.
*/
class Matrix : public MathBase
{
    public:

        /*
        ===================================
             Constructors/Destructors
        ===================================
        */

        /**
        * @brief Default constructor. Initializes all elements to 0.
        */
        Matrix(size_t num_rows, size_t num_cols) : MathBase(), _num_rows(num_rows), _num_cols(num_cols), _rows(num_rows, Vector(num_cols, 0.0)) {}

        /**
        * @brief Constructs a Matrix with the given rows.
        * @param rows Initializer list of rows.
        */
        Matrix(const std::initializer_list<Vector>& rows) : MathBase(), _num_rows(rows.size()), _num_cols(rows.size() == 0 ? 0 : rows.begin()->getSize()), _rows(rows) {
            // check that all rows have the same size
            for (const Vector& row : rows) {
                if (row.getSize() != _num_cols) {
                    throw std::invalid_argument("All rows must have the same size. Row has size " + std::to_string(row.getSize()) + " but expected " + std::to_string(_num_cols));
                }
            }
        }

        /**
        * @brief Default destructor.
        */
        virtual ~Matrix() = default;

        /*
        ===================
             Operators
        ===================
        */

        /**
        * @brief Scalar multiplication.
        * @param c The scalar to multiply by.
        * @return The matrix multiplied element-wise by the scalar.
        */
        Matrix operator*(double c) const {
            Matrix result(_num_rows, _num_cols);
            for (size_t i = 0; i < _num_rows; ++i) {
                result[i] = _rows[i] * c;
            }
            return result;
        }

        /**
        * @brief Scalar division.
        * @param c The scalar to divide by.
        * @throws std::invalid_argument if c is less than epsilon.
        * @return The matrix divided element-wise by the scalar.
        */
        Matrix operator/(double c) const {
            if (std::abs(c) <= _epsilon) {
                throw std::invalid_argument("Division by zero");
            }
            Matrix result(_num_rows, _num_cols);
            for (size_t i = 0; i < _num_rows; ++i) {
                result[i] = _rows[i] / c;
            }
            return result;
        }

        /**
        * @brief Matrix-vector multiplication.
        * @param v The vector to multiply by.
        * @return The resulting vector.
        */
        Vector operator*(const Vector& v) const {
            if (v.getSize() != _num_cols) {
                throw std::invalid_argument("Cannot multiply matrix with " + std::to_string(_num_cols) + " columns by vector of size " + std::to_string(v.getSize()));
            }

            Vector result(_num_rows);
            for (size_t i = 0; i < _num_rows; ++i) {
                result[i] = _rows[i].dot(v);
            }
            return result;
        }

        /**
        * @brief Matrix multiplication.
        * @param m The matrix to multiply by.
        * @return The resulting matrix product.
        */
        Matrix operator*(const Matrix& m) const {
            if (_num_cols != m.getNumRows()) {
                throw std::invalid_argument("Cannot multiply matrix with " + std::to_string(_num_cols) + " columns by matrix with " + std::to_string(m.getNumRows()) + " rows");
            }
            Matrix transposed = m.getTranspose();
            Matrix result(_num_rows, m.getNumCols());
            for (size_t i = 0; i < _num_rows; ++i) {
                for (size_t j = 0; j < m._num_cols; ++j) {
                    result[i][j] = _rows[i].dot(transposed[j]);
                }
            }
            return result;
        }

        /**
        * @brief Row access (const).
        * @param idx The index of the row to access.
        * @throws std::invalid_argument if idx >= N
        * @return The row at the given index.
        */
        const Vector& operator[](size_t idx) const {
            if (idx >= _num_rows) {
                throw std::invalid_argument("invalid index: " + std::to_string(idx));
            }
            return _rows[idx];
        }

        /**
        * @brief Row access (non-const). Allows a row in the Matrix to be modified. The Vector copy assigment operator enforces that the size of the new row is not changed.
        * @param idx The index of the row to access.
        * @throws std::invalid_argument if idx >= N
        * @return Reference to the row at the given index.
        */
        Vector& operator[](size_t idx) {
            if (idx >= _num_rows) {
                throw std::invalid_argument("invalid index: " + std::to_string(idx));
            }
            return _rows[idx];
        }

        /**
        * @brief Equality operator.
        * @param m The matrix to compare to.
        * @return True if the matrices are equal, false otherwise.
        */
        bool operator==(const Matrix& m) const {
            if (_num_rows != m.getNumRows() || _num_cols != m.getNumCols()) {
                throw std::invalid_argument("Cannot compare matrices of different size. Got " + std::to_string(_num_rows) + "x" + std::to_string(_num_cols) + " and " + std::to_string(m.getNumRows()) + "x" + std::to_string(m.getNumCols()));
            }
            for (size_t i = 0; i < _num_rows; ++i) {
                if (_rows[i] != m[i]) {
                    return false;
                }
            }
            return true;
        }

        /**
        * @brief negation operator
        * @param m The matrix to concatenate onto the right of this matrix.
        */
        Matrix operator-() const {
            return *this * -1.0;
        }

        /**
        * @brief horizontal concatenation operator
        * @param m The matrix to concatenate onto the right of this matrix.
        */
        Matrix operator|(const Matrix& m) const {
            return this->horizontalConcatenate(m);
        }

        /*
        ===================
              Methods
        ===================
        */

        /**
        * @brief Returns the number of rows in the Matrix
        * @return The number of rows in the Matrix.
        */
        size_t getNumRows() const { return _num_rows; };

        /**
        * @brief Returns the number of columns in the Matrix
        * @return The number of columns in the Matrix.
        */
        size_t getNumCols() const { return _num_cols; };

        /**
        * @brief Computes the transpose of the matrix.
        * @return The transpose of the matrix.
        */
        Matrix getTranspose() const {
            Matrix result(_num_cols, _num_rows);
            for (size_t i = 0; i < _num_cols; ++i) {
                for (size_t j = 0; j < _num_rows; ++j) {
                    result[i][j] = _rows[j][i];
                }
            }
            return result;
        }

        /**
        * @brief Prints the matrix to stdout.
        */
        void print() const {
            for (size_t i = 0; i < _num_rows; ++i) {
                _rows[i].print();
            }
        }

        /**
        * @brief Returns the matrix as a column-major array for GPU use.
        * @return The matrix as a column-major float array.
        */
        std::vector<float> getColMajorArray() const {
            std::vector<float> array(_num_rows * _num_cols, 0.0);
            for (size_t col = 0; col < _num_cols; ++col) {
                for (size_t row = 0; row < _num_rows; ++row) {
                    array[col * _num_rows + row] = static_cast<float>(_rows[row][col]);
                }
            }
            return array;
        }

        /**
        * @brief horizontal concatenation
        * @param m The matrix to concatenate onto the right of this matrix.
        */
        Matrix horizontalConcatenate(const Matrix& m) const {
            if (_num_rows != m.getNumRows()) {
                throw std::invalid_argument("Cannot right-concatenate matrices with different numbers of rows. Got " + std::to_string(_num_rows) + " and " + std::to_string(m.getNumRows()));
            }
            Matrix concatenated = Matrix(_num_rows, _num_cols + m.getNumCols());
            for (size_t i = 0; i < _num_rows; ++i) {
                concatenated[i] = _rows[i] | m[i];
            }
            return concatenated;
        }

        /**
        * @brief vertical concatenation
        * @param m The matrix to concatenate onto the bottom of this matrix.
        */
        Matrix verticalConcatenate(const Matrix& m) const {
            if (_num_cols != m.getNumCols()) {
                throw std::invalid_argument("Cannot vertical-concatenate matrices with different numbers of cols. Got " + std::to_string(_num_cols) + " and " + std::to_string(m.getNumCols()));
            }
            return (this->getTranspose() | m.getTranspose()).getTranspose();
        }

        static Matrix zeros(int num) {
            // mxn constructor initializes to zeros
            return Matrix(num, num);
        }

    private:

        size_t _num_rows; // number of rows
        size_t _num_cols; // number of columns
        std::vector<Vector> _rows; // The rows of the matrix


};

/**
* @brief Global operator for scalar * matrix.
* @param c The scalar to multiply by.
* @param m The matrix to multiply.
* @return The matrix multiplied element-wise by the scalar.
*/
inline Matrix operator*(double c, const Matrix& m)
{
    return m * c;
}

/**
* @brief Global operator for scalar / matrix (element-wise division).
* @param c The scalar numerator.
* @param m The matrix denominator.
* @return The result of c divided by each element.
*/
inline Matrix operator/(double c, const Matrix& m)
{
    return m / c;
}

} // namespace Stellarium
