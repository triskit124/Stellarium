#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>

#include "MathBase.h"
#include "Vector.h"

namespace Stellarium
{

/**
* @brief A class representing an MxN matrix.
* @tparam M The number of rows of the matrix.
* @tparam N The number of columns of the matrix.
*/
template <size_t M, size_t N>
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
        Matrix() = default;

        /**
        * @brief Constructs a Matrix with the given row vectors.
        * @param rows Array of row vectors.
        */
        Matrix(const std::array<Vector<N>, M>& rows) : _rows(rows) {}

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
            Matrix result;
            for (size_t i = 0; i < M; ++i) {
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
            Matrix result;
            for (size_t i = 0; i < M; ++i) {
                result[i] = _rows[i] / c;
            }
            return result;
        }

        /**
        * @brief Matrix-vector multiplication.
        * @param v The vector to multiply by.
        * @return The resulting vector.
        */
        Vector<M> operator*(const Vector<N>& v) const {
            Vector<M> result;
            for (size_t i = 0; i < M; ++i) {
                result[i] = _rows[i].dot(v);
            }
            return result;
        }

        /**
        * @brief Matrix multiplication.
        * @param m The matrix to multiply by.
        * @return The resulting matrix product.
        */
        template<size_t P>
        Matrix<M, P>  operator*(const Matrix<N, P>& m) const {
            Matrix<P, N> transposed = m.getTranspose();
            Matrix<M, P> result;
            for (size_t i = 0; i < M; ++i) {
                for (size_t j = 0; j < P; ++j) {
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
        const Vector<N>& operator[](size_t idx) const {
            if (idx >= M) {
                throw std::invalid_argument("invalid index: " + std::to_string(idx));
            }
            return _rows[idx];
        }

        /**
        * @brief Row access (non-const).
        * @param idx The index of the row to access.
        * @throws std::invalid_argument if idx >= N
        * @return Reference to the row at the given index.
        */
        Vector<N>& operator[](size_t idx) {
            if (idx >= M) {
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
            // TODO: is there a better way to compare matrices?
            for (size_t i = 0; i < M; ++i) {
                if (_rows[i] != m[i]) {
                    return false;
                }
            }
            return true;
        }

        /*
        ===================
              Methods
        ===================
        */

        /**
        * @brief Computes the transpose of the matrix.
        * @return The transpose of the matrix.
        */
        Matrix<N, M> getTranspose() const {
            Matrix<N, M> result;
            for (size_t i = 0; i < N; ++i) {
                for (size_t j = 0; j < M; ++j) {
                    result[i][j] = _rows[j][i];
                }
            }
            return result;
        }

        /**
        * @brief Prints the matrix to stdout.
        */
        void print() const {
            for (size_t i = 0; i < M; ++i) {
                _rows[i].print();
            }
        }

        /**
        * @brief Returns the matrix as a column-major array for GPU use.
        * @return The matrix as a column-major float array.
        */
        std::array<float, M * N> getColMajorArray() const {
            std::array<float, M * N> array;
            for (size_t col = 0; col < N; ++col) {
                for (size_t row = 0; row < M; ++row) {
                    array[col * M + row] = static_cast<float>(_rows[row][col]);
                }
            }
            return array;
        }

    protected:

        // The rows of the matrix
        std::array<Vector<N>, M> _rows {};

};

/**
* @brief Global operator for scalar * matrix.
* @param c The scalar to multiply by.
* @param m The matrix to multiply.
* @return The matrix multiplied element-wise by the scalar.
*/
template <size_t M, size_t N>
inline Matrix<M, N> operator*(double c, const Matrix<M, N>& m)
{
    return m * c;
}

/**
* @brief Global operator for scalar / matrix (element-wise division).
* @param c The scalar numerator.
* @param m The matrix denominator.
* @return The result of c divided by each element.
*/
template <size_t M, size_t N>
inline Matrix<M, N> operator/(double c, const Matrix<M, N>& m)
{
    return m / c;
}

} // namespace Stellarium
