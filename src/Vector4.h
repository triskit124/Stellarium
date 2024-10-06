#ifndef STELL_VECTOR4
#define STELL_VECTOR4

#include <cmath>
#include <iostream>
#include <stdexcept>

#include "Constants.h"

namespace Stellarium
{

/**
* @brief A class representing a 4D vector.
*/
class Vector4
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
        Vector4();

        /**
        * @brief Constructs a Vector4 object with the given x, y, z, and w values.
        * @param x The x value of the vector.
        * @param y The y value of the vector.
        * @param z The z value of the vector.
        * @param w The w value of the vector.
        */
        Vector4(double x, double y, double z, double w) : _x(x), _y(y), _z(z), _w(w) {};


        /* 
        ===================
             Operators 
        ===================
        */

        /**
        * @brief Operator for element-wise multiplying the vector by a scalar.
        * @param c The scalar to multiply by.
        * @return The vector multiplied element-wise by the scalar.
        */
        Vector4 operator*(double c) const { return Vector4(_x*c, _y*c, _z*c, _w*c); };
        
        /**
        * @brief Operator for element-wise dividing the vector by a scalar.
        * @param c The scalar to divide by.
        * @return The vector divided element-wise by the scalar.
        */
        Vector4 operator/(double c) const { return Vector4(_x/c, _y/c,_z/c, _w/c); };
        
        /**
        * @brief Operator for element-wise adding a scalar to the vector.
        * @param c The scalar to add.
        * @return The vector added e lement-wise by the scalar.
        */
        Vector4 operator+(double c) const { return Vector4(_x+c, _y+c, _z+c, _w+c); };
        
        /**
        * @brief Operator for element-wise subtracting a scalar from the vector.
        * @param c The scalar to subtract.
        * @return The vector subtracted element-wise by the scalar.
        */
        Vector4 operator-(double c) const { return Vector4(_x-c, _y-c, _z-c, _w-c); };
        
        /**
        * @brief Operator for element-wise adding two vectors.
        * @param v The vector to add.
        * @return The element-wise sum of the two vectors.
        */
        Vector4 operator+(const Vector4& v) const { return Vector4(_x+v[0], _y+v[1], _z+v[2], _w+v[3]); };
        
        /**
        * @brief Operator for subtracting two vectors.
        * @param v The vector to subtract.
        * @return The vector subtracted from this vector.
        */
        Vector4 operator-(const Vector4& v) const { return Vector4(_x-v[0], _y-v[1], _z-v[2], _w-v[3]); };
               
        /**
        * @brief Operator for element-wise adding the values of another vector to this vector.
        * @param v The vector to add with.
        */
        void operator+=(const Vector4& v) { _x += v[0]; _y += v[1]; _z += v[2]; _w += v[3]; };
        
        /**
        * @brief Operator for element-wise subtracting the values of another vector to this vector.
        * @param v The vector to subtract with.
        */
        void operator-=(const Vector4& v) { _x -= v[0]; _y -= v[1]; _z -= v[2]; _w -= v[3]; };
        
        /**
        * @brief Operator for checking equality of two vectors.
        * @param v The vector to check equality with.
        * @return Whether the two vectors are equal within this->epsilon().
        */
        bool operator==(const Vector4& v) const { 
            return 
                std::abs(_x - v[0]) <= _epsilon 
                && std::abs(_y - v[1]) <= _epsilon 
                && std::abs(_z - v[2]) <= _epsilon
                && std::abs(_w - v[3]) <= _epsilon;
        };

        /**
        * @brief Unary operator for negating the vector.
        * @return The negated vector.
        */
        Vector4 operator-() const { return Vector4(-_x, -_y, -_z, -_w); };

        /**
        * @brief Operator for accessing the x, y, and z values of the vector.
        * @param idx The index of the value to access.
        * @return The value at the given index.
        */
        double operator[](int idx) const {
            if (idx == 0)
            {
                return _x;
            }
            if (idx == 1)
            {
                return _y;
            }
            if (idx == 2)
            {
                return _z;
            }
            if (idx == 3)
            {
                return _w;
            }
            throw std::invalid_argument("invalid index"); 
        }

        /* 
        ===================
              Methods 
        ===================
        */

        /**
        * @brief Gets pointer to the x value of the vector.
        * @return Pointer to the x value of the vector.
        */
        double* x() { return &_x; };
        
        /**
        * @brief Gets pointer to the y value of the vector.
        * @return Pointer to the y value of the vector.
        */
        double* y() { return &_y; };
        
        /**
        * @brief Gets pointer to the z value of the vector.
        * @return Pointer to the z value of the vector.
        */
        double* z() { return &_z; };

        /**
        * @brief Gets pointer to the w value of the vector.
        * @return Pointer to the w value of the vector.
        */
        double* w() { return &_w; };

        /**
        * @brief Gets the epsilon value for floating point comparisons.
        * @return The epsilon value.
        */
        double epsilon() const { return _epsilon; };

        /**
        * @brief Sets the epsilon value for floating point comparisons.
        * @param e The new epsilon value.
        */
        void epsilon(double e) { _epsilon = e; };

        /**
        * @brief Prints the x, y, and z values of the vector to stdout.
        */
        void print() const {
            std::cout << "x: " << _x << " y: " << _y << " z: " << _z << " w: " << _w << "\n";
        }

        /**
        * @brief Returns the dot product of the vector with another vector.
        * @param v The vector to dot with.
        * @return The dot product of the two vectors.
        */
        double dot(const Vector4& v) const { return _x*v[0] + _y*v[1] + _z*v[2] + _w*v[3]; };

    protected:
    
    private:
        /**
        * @brief The x value of the vector.
        */
        double _x = 0.0;
        
        /**
        * @brief The y value of the vector.
        */
        double _y = 0.0;
        
        /**
        * @brief The z value of the vector.
        */
        double _z = 0.0;

        /**
        * @brief The w value of the vector.
        */
        double _w = 0.0;
        
        /**
        * @brief The epsilon value for floating point comparisons.
        */
        double _epsilon = STELL_EPSILON;
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

#endif // STELL_VECTOR4