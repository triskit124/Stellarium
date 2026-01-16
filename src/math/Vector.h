#pragma once

#include <array>
#include <cmath>
#include <iostream>
#include <stdexcept>

#include "MathBase.h"

namespace Stellarium
{

/**
* @brief A class representing an N-dimensional vector.
*/
template <size_t N>
class Vector : public MathBase
{
    public:

        /*
        ===================================
             Constructors/Desctructors
        ===================================
        */

        /**
        * @brief Default constructor for the Vector object.
        */
        Vector() = default;

        /**
        * @brief Constructs a Vector with the given values.
        */
        Vector(const std::array<double, N>& values) : _data(values) {}

        /**
        * @brief Default destructor for the Vector object.
        */
        virtual ~Vector() = default;


        /*
        ===================
             Operators
        ===================
        */

        Vector operator*(double c) const {
            Vector result = *this;
            for (size_t i = 0; i < N; ++i) {
                result[i] *= c;
            }
            return result;
        }

        Vector operator/(double c) const {
            if (std::abs(c) <= _epsilon) {
                throw std::invalid_argument("Division by zero");
            }
            Vector result = *this;
            for (size_t i = 0; i < N; ++i) {
                result[i] /= c;
            }
            return result;
        }

        Vector operator+(double c) const {
            Vector result = *this;
            for (size_t i = 0; i < N; ++i) {
                result[i] += c;
            }
            return result;
        }

        Vector operator-(double c) const {
            Vector result = *this;
            for (size_t i = 0; i < N; ++i) {
                result[i] -= c;
            }
            return result;
        }

        double operator*(const Vector& v) const {
            return this->dot(v);
        }

        Vector operator+(const Vector& v) const {
            Vector result = *this;
            for (size_t i = 0; i < N; ++i) {
                result[i] += v[i];
            }
            return result;
        }

        Vector operator-(const Vector& v) const {
            Vector result = *this;
            for (size_t i = 0; i < N; ++i) {
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
            for (size_t i = 0; i < N; ++i) {
                if (std::abs((*this)[i] - v[i]) > _epsilon) {
                    return false;
                }
            }
            return true;
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
            if (idx >= N) {
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
            if (idx >= N) {
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
        static size_t size() { return N; }

        /**
        * @brief Prints the values of the vector to stdout.
        */
        void print(const std::string& s = "") const {
            std::cout << s << "(";
            for (size_t i = 0; i < N; ++i) {
                std::cout << _data[i];
                if (i < N - 1) std::cout << ", ";
            }
            std::cout << ")" << std::endl;
        }

        /**
        * @brief Returns the dot product of the vector with another vector.
        * @param v The vector to dot with.
        * @return The dot product of the two vectors.
        */
        double dot(const Vector& v) const {
            double result = 0.0;
            for (size_t i = 0; i < N; ++i) {
                result += (*this)[i] * v[i];
            }
            return result;
        }

        /**
        * @brief Returns the norm of the vector.
        */
        double norm() const {
            return std::sqrt(this->dot(*this));
        }

        /**
        * @brief Normalizes the vector in place.
        */
        void normalize() {
            double n = norm();
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
            return std::abs(norm() - 1.0) <= _epsilon;
        }

        protected:

            std::array<double, N> _data { 0.0 };

};


/**
* @brief Global operator for element-wise multiplying a vector by a scalar.
* @param v The vector to multiply by.
* @param c The scalar to multiply by.
* @return The vector multiplied element-wise by the scalar.
*/
template <size_t N>
inline Vector<N> operator*(double c, const Vector<N>& v)
{
    return v * c;
};

/**
* @brief Global operator for element-wise adding a scalar to a vector.
* @param v The vector to add to.
* @param c The scalar to add.
* @return The vector added element-wise by the scalar.
*/
template <size_t N>
inline Vector<N> operator+(double c, const Vector<N>& v)
{
    return v + c;
};


} // namespace Stellarium
