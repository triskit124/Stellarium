#ifndef STELL_VECTOR2_H
#define STELL_VECTOR2_H

#include <cmath>
#include <iostream>
#include <stdexcept>

#include "MathBase.h"

namespace Stellarium
{

/**
* @brief A class representing a 2D vector.
*/
class Vector2: public MathBase
{   
    public:

        /* 
        ===================================
             Constructors/Desctructors 
        ===================================
        */

        /**
        * @brief Default constructor for the Vector2 object.
        */
        Vector2() = default;

        /**
        * @brief Constructs a Vector2 object with the given x, y, and z values.
        * @param x The x value of the vector.
        * @param y The y value of the vector.
        * @param z The z value of the vector.
        */
        Vector2(double x, double y) : x(x), y(y) {};

        /*
        ====================
            Data Members
        ====================
        */

        /**
        * @brief The x value of the vector.
        */
        double x = 0.0;
        
        /**
        * @brief The y value of the vector.
        */
        double y = 0.0;
        
        /* 
        ===================
             Operators 
        ===================
        */

        Vector2 operator*(double c) const { return Vector2(x*c, y*c); };
        Vector2 operator/(double c) const { return Vector2(x/c, y/c); };
        Vector2 operator+(double c) const { return Vector2(x+c, y+c); };
        Vector2 operator-(double c) const { return Vector2(x-c, y-c); };
        
        double operator*(const Vector2& v) const { return this->dot(v); };
        Vector2 operator+(const Vector2& v) const { return Vector2(x+v.x, y+v.y); };
        Vector2 operator-(const Vector2& v) const { return Vector2(x-v.x, y-v.y); };

        void operator*=(double c) { x *= c; y *= c; };
        void operator/=(double c) { 
            if (std::abs(c) <= _epsilon) {
                throw std::invalid_argument("Division by zero in Vector2 operator/=");
            }
            x /= c; y /= c; 
        };

        void operator+=(double c) { x += c; y += c; };
        void operator-=(double c) { x -= c; y -= c; };

        void operator+=(const Vector2& v) { x += v.x; y += v.y; };
        void operator-=(const Vector2& v) { x -= v.x; y -= v.y; };
        
        bool operator==(const Vector2& v) const { 
            return 
                std::abs(x - v.x) <= _epsilon 
                && std::abs(y - v.y) <= _epsilon;
        };

        bool operator!=(const Vector2& v) const { 
            return !(*this == v); 
        };

        Vector2 operator-() const { return Vector2(-x, -y); };

        /**
        * @brief Operator for accessing the x, y, and z values of the vector.
        * @param idx The index of the value to access.
        * @return The value at the given index.
        */
        double operator[](unsigned int idx) const {
            if (idx == 0)
            {
                return x;
            }
            if (idx == 1)
            {
                return y;
            }
            throw std::invalid_argument("invalid index: " + std::to_string(idx)); 
        }

        /**
        * @brief Operator for accessing the x, y, and z values of the vector.
        * @param idx The index of the value to access.
        * @return The value at the given index.
        */
        double& operator[](unsigned int idx) {
            if (idx == 0)
            {
                return x;
            }
            if (idx == 1)
            {
                return y;
            }
            throw std::invalid_argument("invalid index: " + std::to_string(idx)); 
        }

        /* 
        ===================
              Methods 
        ===================
        */

        constexpr static unsigned int size() { return 2; }

        /**
        * @brief Prints the x, y, and z values of the vector to stdout.
        */
        void print(const std::string& s = "") const {
            std::cout << s << "x: " << x << " y: " << y << std::endl;
        }

        /**
        * @brief Returns the dot product of the vector with another vector.
        * @param v The vector to dot with.
        * @return The dot product of the two vectors.
        */
        double dot(const Vector2& v) const { return x*v.x + y*v.y; };

        /**
        * @brief Returns the norm of the vector.
        */
        double norm() const { 
            return std::sqrt(pow(x, 2) + pow(y, 2)); 
        };

        /**
        * @brief Normalizes the vector in place.
        */
        void normalize() { 
            double n = norm();
            if (!isUnit() && n > _epsilon)
            {
                x /= n;
                y /= n;
            }
        }

        /**
        * @brief Returns a copy of this vector that is normalized. Does not modify the existed vector
        * @return The normalized vector.
        */
        Vector2 getNormalized() const {
            Vector2 v = *this;
            v.normalize();
            return v;
        }

        /**
        * @brief Checks if the vector is a unit vector.
        * @return Whether the vector is a unit vector.
        */
        bool isUnit() const { return std::abs(norm() - 1.0) <= _epsilon; };
    
};

/**
* @brief Global operator for element-wise multiplying a vector by a scalar.
* @param v The vector to multiply by.
* @param c The scalar to multiply by.
* @return The vector multiplied element-wise by the scalar.
*/
inline Vector2 operator*(double c, const Vector2& v)
{
    return v*c;
};

/**
* @brief Global operator for element-wise adding a scalar to a vector.
* @param v The vector to add to.
* @param c The scalar to add.
* @return The vector added element-wise by the scalar.
*/
inline Vector2 operator+(double c, const Vector2& v)
{
    return v+c;
};

} // namespace Stellarium

#endif // STELL_VECTOR2_H