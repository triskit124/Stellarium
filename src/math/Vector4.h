#ifndef STELL_VECTOR4_H
#define STELL_VECTOR4_H

#include <cmath>
#include <iostream>
#include <stdexcept>

#include "MathBase.h"

namespace Stellarium
{

/**
* @brief A class representing a 4D vector.
*/
class Vector4: public MathBase
{   
    public:

        /* 
        ===================================
             Constructors/Desctructors 
        ===================================
        */

        /**
        * @brief Default constructor for the Vector4 object.
        */
        Vector4() = default;

        /**
        * @brief Constructs a Vector4 object with the given x, y, z, and w values.
        * @param x The x value of the vector.
        * @param y The y value of the vector.
        * @param z The z value of the vector.
        * @param w The w value of the vector.
        */
        Vector4(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) {};

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
        
        /**
        * @brief The z value of the vector.
        */
        double z = 0.0;

        /**
        * @brief The w value of the vector.
        */
        double w = 0.0;

        /* 
        ===================
             Operators 
        ===================
        */

        Vector4 operator*(double c) const { return Vector4(x*c, y*c, z*c, w*c); };
        Vector4 operator/(double c) const { return Vector4(x/c, y/c,z/c, w/c); };
        Vector4 operator+(double c) const { return Vector4(x+c, y+c, z+c, w+c); };
        Vector4 operator-(double c) const { return Vector4(x-c, y-c, z-c, w-c); };
        
        double operator*(const Vector4& v) const { return this->dot(v); };
        Vector4 operator+(const Vector4& v) const { return Vector4(x+v.x, y+v.y, z+v.z, w+v.w); };
        Vector4 operator-(const Vector4& v) const { return Vector4(x-v.x, y-v.y, z-v.z, w-v.w); };
        
        void operator*=(double c) { x *= c; y *= c; z *= c; w *= c; };
        void operator/=(double c) { 
            if (std::abs(c) <= _epsilon) {
                throw std::invalid_argument("Division by zero in Vector4 operator/=");
            }
            x /= c; y /= c; z /= c; w /= c; 
        };

        void operator+=(const Vector4& v) { x += v.x; y += v.y; z += v.z; w += v.w; };
        void operator-=(const Vector4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; };

        bool operator==(const Vector4& v) const { 
            return 
                std::abs(x - v.x) <= _epsilon 
                && std::abs(y - v.y) <= _epsilon 
                && std::abs(z - v.z) <= _epsilon
                && std::abs(w - v.w) <= _epsilon;
        };

        bool operator!=(const Vector4& v) const { 
            return !(*this == v); 
        };

        Vector4 operator-() const { return Vector4(-x, -y, -z, -w); };

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
            if (idx == 2)
            {
                return z;
            }
            if (idx == 3)
            {
                return w;
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
            if (idx == 2)
            {
                return z;
            }
            if (idx == 3)
            {
                return w;
            }
            throw std::invalid_argument("invalid index: " + std::to_string(idx)); 
        }

        /* 
        ===================
              Methods 
        ===================
        */

        constexpr static unsigned int size() { return 4; }

        /**
        * @brief Prints the values of the vector to stdout.
        */
        void print(const std::string& s = "") const {
            std::cout << s << "x: " << x << " y: " << y << " z: " << z << " w:" << w << std::endl;
        }

        /**
        * @brief Returns the dot product of the vector with another vector.
        * @param v The vector to dot with.
        * @return The dot product of the two vectors.
        */
        double dot(const Vector4& v) const { return x*v.x + y*v.y + z*v.z + w*v.w; };

};

/**
* @brief Global operator for element-wise multiplying a vector by a scalar.
* @param v The vector to multiply by.
* @param c The scalar to multiply by.
* @return The vector multiplied element-wise by the scalar.
*/
inline Vector4 operator*(double c, const Vector4& v)
{
    return v*c;
};

/**
* @brief Global operator for element-wise adding a scalar to a vector.
* @param v The vector to add to.
* @param c The scalar to add.
* @return The vector added element-wise by the scalar.
*/
inline Vector4 operator+(double c, const Vector4& v)
{
    return v+c;
};

} // namespace Stellarium

#endif // STELL_VECTOR4_H