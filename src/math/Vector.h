#pragma once

#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "MathBase.h"

namespace Stellarium
{

/**
* @brief A class representing an N-dimensional vector. Size is given at construction time and cannot
* be changed afterwards, with one exception: a default-constructed (size 0, "unsized") Vector adopts
* the size of the first vector assigned to it. See operator=.
*/
class Vector : public MathBase
{
    public:

        /*
        ===================================
             Constructors/Destructors
        ===================================
        */

        /**
        * @brief Constructs a Vector object with the given size. Optionally initialize all elements to a value.
        * @param size The size of the vector.
        * @param val Value to assign to all elements. Defaults to 0.
        */
        Vector(size_t size = 0, double val = 0.0) : MathBase(), _size(size), _data(size, val) { };

        /**
        * @brief Constructs a Vector with the given values. The size of the vector is determined by the number of values given.
         * @param values The values to initialize the vector with.
        */
        Vector(const std::initializer_list<double>& values) : MathBase(), _size(values.size()), _data(values) { };

        /**
        * @brief Copy constructor. Explicitly defaulted because we have a user-defined copy-assignment operator.
        */
        Vector(const Vector& other) = default; 

        /**
        * @brief Move constructor. Explicitly defaulted because we have a user-defined copy-assignment operator.
        */
        Vector(Vector&& other) noexcept = default;

        /**
        * @brief Default destructor for the Vector object.
        */
        virtual ~Vector() = default;


        /*
        ===================
             Operators
        ===================
        */

        /**
        * @brief Copy assignment operator. Performs a size check before copying.
        *
        * A default-constructed Vector has size 0 and is treated as "unsized": assigning to it
        * adopts the size of the right-hand side. This is what makes accumulation patterns such as
        * `Vector acc; acc = acc.concatenate(chunk);` and struct fields like Joint::Info::q_init usable.
        * Once a Vector is non-empty its size is fixed, and assigning a differently-sized vector
        * to it throws.
        *
        * @param rhs The vector to copy from.
        * @throws std::invalid_argument if this vector is non-empty and the sizes do not match.
        * @return Reference to this vector after assignment.
        */
        Vector& operator=(const Vector& rhs) {
            _checkAssignable(rhs.getSize());
            _size = rhs._size;
            _data = rhs._data;
            return *this;
        }

        /**
        * @brief Move assignment operator. Follows the same sizing rule as the copy-assignment
        * operator: an empty (size 0) vector adopts the size of the right-hand side, otherwise the
        * sizes must match.
        * @param rhs The vector to move from.
        * @throws std::invalid_argument if this vector is non-empty and the sizes do not match.
        * @return Reference to this vector after assignment.
        */
        Vector& operator=(Vector&& rhs) {
            _checkAssignable(rhs.getSize());
            _size = rhs._size;
            _data = rhs._data; // copy, not move: leaving rhs with a size but no data would break its invariant
            return *this;
        } 

        Vector operator*(double c) const {
            Vector result = *this;
            for (size_t i = 0; i < _size; ++i) {
                result[i] *= c;
            }
            return result;
        }

        Vector operator/(double c) const {
            if (std::abs(c) <= _epsilon) {
                throw std::invalid_argument("Division by zero");
            }
            Vector result = *this;
            for (size_t i = 0; i < _size; ++i) {
                result[i] /= c;
            }
            return result;
        }

        Vector operator+(double c) const {
            Vector result = *this;
            for (size_t i = 0; i < _size; ++i) {
                result[i] += c;
            }
            return result;
        }

        Vector operator-(double c) const {
            Vector result = *this;
            for (size_t i = 0; i < _size; ++i) {
                result[i] -= c;
            }
            return result;
        }

        double operator*(const Vector& v) const {
            return this->dot(v);
        }

        Vector operator+(const Vector& v) const {
            if (_size != v.getSize())
            {
                throw std::invalid_argument("Cannot add two Vectors of different size. Got " + std::to_string(getSize()) + " and " + std::to_string(v.getSize()));
            }
            Vector result = *this;
            for (size_t i = 0; i < _size; ++i) {
                result[i] += v[i];
            }
            return result;
        }

        Vector operator-(const Vector& v) const {
            if (_size != v.getSize())
            {
                throw std::invalid_argument("Cannot subtract two Vectors of different size. Got " + std::to_string(getSize()) + " and " + std::to_string(v.getSize()));
            }
            Vector result = *this;
            for (size_t i = 0; i < _size; ++i) {
                result[i] -= v[i];
            }
            return result;
        }

        void operator*=(double c) {
            *this = *this * c;
        }

        void operator/=(double c) {
            if (std::abs(c) <= _epsilon) {
                throw std::invalid_argument("Division by zero in Vector operator/=");
            }
            *this = *this / c;
        }

        void operator+=(double c) {
            *this = *this + c;
        }

        void operator-=(double c) {
            *this = *this - c;
        }

        void operator+=(const Vector& v) {
            *this = *this + v;
        }

        void operator-=(const Vector& v) {
            *this = *this - v;
        }

        bool operator==(const Vector& v) const {
            if (_size != v.getSize())
            {
                throw std::invalid_argument("Cannot equate two Vectors of different size. Got " + std::to_string(getSize()) + " and " + std::to_string(v.getSize()));
            }
            for (size_t i = 0; i < _size; ++i) {
                if (std::abs((*this)[i] - v[i]) > _epsilon) {
                    return false;
                }
            }
            return true;
        }

        bool operator!=(const Vector& v) const {
            return !(*this == v);
        }

        Vector operator-() const {
            return (*this * -1.0);
        }

        /**
        * @brief Operator for accessing the values of the vector.
        * @param idx The index of the value to access.
        * @return The value at the given index.
        */
        double operator[](size_t idx) const {
            if (idx >= _size) {
                throw std::invalid_argument("invalid index: " + std::to_string(idx));
            }
            return _data[idx];
        }

        /**
        * @brief Operator for accessing the values of the vector.
        * @param idx The index of the value to access.
        * @return The value at the given index.
        */
        double& operator[](size_t idx) {
            if (idx >= _size) {
                throw std::invalid_argument("invalid index: " + std::to_string(idx));
            }
            return _data[idx];
        }

        /*
        ===================
              Methods
        ===================
        */

        /**
        * @brief Returns the size of the vector.
        */
        size_t getSize() const { return _size; }

        /**
        * @brief Prints the values of the vector to stdout.
        */
        void print(const std::string& s = "") const {
            std::cout << s << "(";
            for (size_t i = 0; i < _size; ++i) {
                std::cout << _data[i];
                if (i < _size - 1) std::cout << ", ";
            }
            std::cout << ")" << std::endl;
        }

        /**
        * @brief Returns the dot product of the vector with another vector.
        * @param v The vector to dot with.
        * @return The dot product of the two vectors.
        */
        double dot(const Vector& v) const {
            if (_size != v.getSize())
            {
                throw std::invalid_argument("Cannot dot two Vectors of different size. Got " + std::to_string(getSize()) + " and " + std::to_string(v.getSize()));
            }
            double result = 0.0;
            for (size_t i = 0; i < _size; ++i) {
                result += (*this)[i] * v[i];
            }
            return result;
        }

        /**
        * @brief Returns the norm of the vector.
        */
        double getNorm() const {
            return std::sqrt(this->dot(*this));
        }

        /**
        * @brief Normalizes the vector in place.
        */
        void normalize() {
            double n = getNorm();
            if (!isUnit() && n > _epsilon) {
                *this /= n;
            }
        }

        /**
        * @brief Returns a copy of this vector that is normalized. Does not modify the existed vector
        * @return The normalized vector.
        */
        Vector getNormalized() const {
            Vector v = *this;
            v.normalize();
            return v;
        }

        /**
        * @brief Checks if the vector is a unit vector.
        * @return Whether the vector is a unit vector.
        */
        bool isUnit() const {
            return std::abs(getNorm() - 1.0) <= _epsilon;
        }

        std::vector<double> getData() const { return _data; };

        /**
        * @brief concatenate this vector with another vector
        * @param m The vector to concatenate onto the end of this vector.
        */
        Vector concatenate(const Vector& m) const {
            Vector concatenated = Vector(_size + m.getSize());
            for (size_t i = 0; i < _size; ++i) {
                concatenated[i] = _data[i];
            }
            for (size_t i = 0; i < m.getSize(); ++i) {
                concatenated[i + _size] = m[i];
            }
            return concatenated;
        }

        private:

            /**
            * @brief Throws unless this vector can be assigned a vector of the given size, i.e.
            * unless it is empty ("unsized") or already has that exact size.
            */
            void _checkAssignable(size_t rhs_size) const {
                if (_size != 0 && rhs_size != _size) {
                    throw std::invalid_argument("Cannot assign vector of size " + std::to_string(rhs_size) + " to vector of size " + std::to_string(_size));
                }
            }

            size_t _size; // number of elements in the vector
            std::vector<double> _data; // the values of the vector

};


/**
* @brief Global operator for element-wise multiplying a vector by a scalar.
* @param v The vector to multiply by.
* @param c The scalar to multiply by.
* @return The vector multiplied element-wise by the scalar.
*/
inline Vector operator*(double c, const Vector& v)
{
    return v * c;
};

/**
* @brief Global operator for element-wise adding a scalar to a vector.
* @param v The vector to add to.
* @param c The scalar to add.
* @return The vector added element-wise by the scalar.
*/
inline Vector operator+(double c, const Vector& v)
{
    return v + c;
};


} // namespace Stellarium
