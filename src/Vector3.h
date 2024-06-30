#ifndef STELL_VECTOR3
#define STELL_VECTOR3

#include <cmath>
#include <iostream>
#include <stdexcept>

#include "Constants.h"

namespace Stellarium
{

/**
* @brief A class representing a 3D vector.
*/
class Vector3
{   
    public:

        /* 
        ===================================
             Constructors/Desctructors 
        ===================================
        */

        /**
        * @brief Default constructor for the Vector3 object.
        */
        Vector3();

        /**
        * @brief Constructs a Vector3 object with the given x, y, and z values.
        * @param x The x value of the vector.
        * @param y The y value of the vector.
        * @param z The z value of the vector.
        */
        Vector3(double x, double y, double z) : _x(x), _y(y), _z(z) {};

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
        Vector3 operator*(double c) const { return Vector3(_x*c, _y*c, _z*c); };
        
        /**
        * @brief Operator for element-wise dividing the vector by a scalar.
        * @param c The scalar to divide by.
        * @return The vector divided element-wise by the scalar.
        */
        Vector3 operator/(double c) const { return Vector3(_x/c, _y/c,_z/c); };
        
        /**
        * @brief Operator for element-wise adding a scalar to the vector.
        * @param c The scalar to add.
        * @return The vector added e lement-wise by the scalar.
        */
        Vector3 operator+(double c) const { return Vector3(_x+c, _y+c, _z+c); };
        
        /**
        * @brief Operator for element-wise subtracting a scalar from the vector.
        * @param c The scalar to subtract.
        * @return The vector subtracted element-wise by the scalar.
        */
        Vector3 operator-(double c) const { return Vector3(_x-c, _y-c, _z-c); };
        
        /**
        * @brief Operator for element-wise adding two vectors.
        * @param v The vector to add.
        * @return The element-wise sum of the two vectors.
        */
        Vector3 operator+(const Vector3& v) const { return Vector3(_x+v[0], _y+v[1], _z+v[2]); };
        
        /**
        * @brief Operator for subtracting two vectors.
        * @param v The vector to subtract.
        * @return The vector subtracted from this vector.
        */
        Vector3 operator-(const Vector3& v) const { return Vector3(_x-v[0], _y-v[1], _z-v[2]); };
               
        /**
        * @brief Operator for element-wise adding the values of another vector to this vector.
        * @param v The vector to add with.
        */
        void operator+=(const Vector3& v) { _x += v[0]; _y += v[1]; _z += v[2]; };
        
        /**
        * @brief Operator for element-wise subtracting the values of another vector to this vector.
        * @param v The vector to subtract with.
        */
        void operator-=(const Vector3& v) { _x -= v[0]; _y -= v[1]; _z -= v[2]; };
        
        /**
        * @brief Operator for checking equality of two vectors.
        * @param v The vector to check equality with.
        * @return Whether the two vectors are equal within this->epsilon().
        */
        bool operator==(const Vector3& v) const { 
            return 
                std::abs(_x - v[0]) <= _epsilon 
                && std::abs(_y - v[1]) <= _epsilon 
                && std::abs(_z - v[2]) <= _epsilon;
        };

        /**
        * @brief Unary operator for negating the vector.
        * @return The negated vector.
        */
        Vector3 operator-() const { return Vector3(-_x, -_y, -_z); };

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
            std::cout << "x: " << _x << " y: " << _y << " z: " << _z << std::endl;
        }

        /**
        * @brief Returns the dot product of the vector with another vector.
        * @param v The vector to dot with.
        * @return The dot product of the two vectors.
        */
        double dot(const Vector3& v) const { return _x*v[0] + _y*v[1] + _z*v[2]; };

        /**
        * @brief Returns the cross product of the vector with another vector.
        * @param v The vector to cross with.
        * @return The cross product of the two vectors.
        */
        Vector3 cross(const Vector3& v) const {
            return Vector3(
                _y*v[2] - _z*v[1],
                _z*v[0] - _x*v[2],
                _x*v[1] - _y*v[0]
            );
        };

        /**
        * @brief Returns the norm of the vector.
        */
        double norm() const { 
            return std::sqrt(pow(_x, 2) + pow(_y, 2) + pow(_z, 2)); 
        };

        /**
        * @brief Normalizes the vector in place.
        */
        void normalize() { 
            double n = norm();
            if (n != 1.0)
            {
                _x /= n;
                _y /= n;
                _z /= n;
            }
        }

        /**
        * @brief Returns a copy of this vector that is normalized. Does not modify the existed vector
        * @return The normalized vector.
        */
        Vector3 getNormalized() const {
            Vector3 v = *this;
            v.normalize();
            return v;
        }

        /**
        * @brief Checks if the vector is a unit vector.
        * @return Whether the vector is a unit vector.
        */
        bool isUnit() const { return std::abs(norm() - 1.0) <= _epsilon; };
    
    protected:
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
        * @brief The epsilon value for floating point comparisons.
        */
        double _epsilon = STELL_EPSILON;

    private:
};

/**
* @brief Global operator for element-wise multiplying a vector by a scalar.
* @param v The vector to multiply by.
* @param c The scalar to multiply by.
* @return The vector multiplied element-wise by the scalar.
*/
inline Vector3 operator*(double c, const Vector3& v)
{
    return v*c;
};

/**
* @brief Global operator for element-wise adding a scalar to a vector.
* @param v The vector to add to.
* @param c The scalar to add.
* @return The vector added element-wise by the scalar.
*/
inline Vector3 operator+(double c, const Vector3& v)
{
    return v+c;
};

} // namespace Stellarium

#endif // STELL_VECTOR3