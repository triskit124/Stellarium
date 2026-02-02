#pragma once

#include <array>
#include <cmath>
#include <stdexcept>

#include "MathBase.h"
#include "Vector.h"

namespace Stellarium
{

/**
* @brief A class representing an NxN matrix.
* @tparam N The dimension of the square matrix.
* @tparam VecType The vector type used for rows (defaults to Vector<N>).
*/
template <size_t N, typename VecType = Vector<N>>
class Matrix : public MathBase
{
    public:

        /*
        ===================================
             Constructors/Destructors
        ===================================
        */

        /**
        * @brief Default constructor - initializes to identity matrix.
        */
        Matrix() {
            for (size_t i = 0; i < N; ++i) {
                _rows[i][i] = 1.0;
            }
        }

        /**
        * @brief Constructs a Matrix with the given row vectors.
        * @param rows Array of row vectors.
        */
        Matrix(const std::array<VecType, N>& rows) : _rows(rows) {}

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
            for (size_t i = 0; i < N; ++i) {
                result[i] = _rows[i] * c;
            }
            return result;
        }

        /**
        * @brief Scalar division.
        * @param c The scalar to divide by.
        * @return The matrix divided element-wise by the scalar.
        */
        Matrix operator/(double c) const {
            if (std::abs(c) <= _epsilon) {
                throw std::invalid_argument("Division by zero");
            }
            Matrix result;
            for (size_t i = 0; i < N; ++i) {
                result[i] = _rows[i] / c;
            }
            return result;
        }

        /**
        * @brief Matrix-vector multiplication.
        * @param v The vector to multiply by.
        * @return The resulting vector.
        */
        VecType operator*(const VecType& v) const {
            VecType result;
            for (size_t i = 0; i < N; ++i) {
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
            Matrix transposed = m.getTranspose();
            Matrix result;
            for (size_t i = 0; i < N; ++i) {
                for (size_t j = 0; j < N; ++j) {
                    result[i][j] = _rows[i].dot(transposed[j]);
                }
            }
            return result;
        }

        /**
        * @brief Row access (const).
        * @param idx The index of the row to access.
        * @return The row at the given index.
        */
        const VecType& operator[](size_t idx) const {
            if (idx >= N) {
                throw std::invalid_argument("invalid index: " + std::to_string(idx));
            }
            return _rows[idx];
        }

        /**
        * @brief Row access (non-const).
        * @param idx The index of the row to access.
        * @return Reference to the row at the given index.
        */
        VecType& operator[](size_t idx) {
            if (idx >= N) {
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
            for (size_t i = 0; i < N; ++i) {
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
        * @brief Returns the size (dimension) of the matrix.
        */
        static constexpr size_t size() { return N; }

        /**
        * @brief Computes the transpose of the matrix.
        * @return The transpose of the matrix.
        */
        Matrix getTranspose() const {
            Matrix result;
            for (size_t i = 0; i < N; ++i) {
                for (size_t j = 0; j < N; ++j) {
                    result[i][j] = _rows[j][i];
                }
            }
            return result;
        }

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
            return (*this * this->getTranspose()) == Matrix();
        }

        /**
        * @brief Prints the matrix to stdout.
        */
        void print() const {
            for (size_t i = 0; i < N; ++i) {
                _rows[i].print();
            }
        }

        /**
        * @brief Returns the matrix as a column-major array for GPU use.
        * @return The matrix as a column-major float array.
        */
        std::array<float, N * VecType::size()> getColMajorArray() const {
            constexpr size_t M = VecType::size();
            std::array<float, N * M> array;
            for (size_t col = 0; col < M; ++col) {
                for (size_t row = 0; row < N; ++row) {
                    array[col * N + row] = static_cast<float>(_rows[row][col]);
                }
            }
            return array;
        }

    protected:

        // The rows of the matrix
        std::array<VecType, N> _rows {};

};

/**
* @brief Global operator for scalar * matrix.
* @param c The scalar to multiply by.
* @param m The matrix to multiply.
* @return The matrix multiplied element-wise by the scalar.
*/
template <size_t N, typename VecType>
inline Matrix<N, VecType> operator*(double c, const Matrix<N, VecType>& m)
{
    return m * c;
}

/**
* @brief Global operator for scalar / matrix (element-wise division).
* @param c The scalar numerator.
* @param m The matrix denominator.
* @return The result of c divided by each element.
*/
template <size_t N, typename VecType>
inline Matrix<N, VecType> operator/(double c, const Matrix<N, VecType>& m)
{
    return m / c;
}

} // namespace Stellarium
