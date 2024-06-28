#ifndef STELL_MATH
#define STELL_MATH

#include "Constants.h"

#include <array>
#include <cmath>
#include <stdexcept>
#include <string>
#include <iostream>

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

        /**
        * @brief Copy constructor.
        */
        Vector3(const Vector3& v) : _x(v[0]), _y(v[1]), _z(v[2]) {};

        /**
        * @brief Default destructor.
        */
        ~Vector3() {};

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
        Vector3 operator+(Vector3 v) const { return Vector3(_x+v[0], _y+v[1], _z+v[2]); };
        
        /**
        * @brief Operator for subtracting two vectors.
        * @param v The vector to subtract.
        * @return The vector subtracted from this vector.
        */
        Vector3 operator-(Vector3 v) const { return Vector3(_x-v[0], _y-v[1], _z-v[2]); };
        
        /**
        * @brief Operator for assigning a vector to another vector.
        * @param v The vector to assign.
        */
        void operator=(Vector3 v) { _x = v[0]; _y = v[1]; _z = v[2]; };
        
        /**
        * @brief Operator for element-wise adding the values of another vector to this vector.
        * @param v The vector to add with.
        */
        void operator+=(Vector3 v) { _x += v[0]; _y += v[1]; _z += v[2]; };
        
        /**
        * @brief Operator for element-wise subtracting the values of another vector to this vector.
        * @param v The vector to subtract with.
        */
        void operator-=(Vector3 v) { _x -= v[0]; _y -= v[1]; _z -= v[2]; };
        
        /**
        * @brief Operator for checking equality of two vectors.
        * @param v The vector to check equality with.
        * @return Whether the two vectors are equal within this->epsilon().
        */
        bool operator==(Vector3 v) const { 
            return 
                std::abs(_x - v[0]) <= _epsilon 
                && std::abs(_y - v[1]) <= _epsilon 
                && std::abs(_z - v[2]) <= _epsilon;
        };

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
        * @brief Normalizes the vector.
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
* @brief A class representing a quaternion.
*/
class Quaternion
{    
    public:

        /* 
        ===================================
             Constructors/Desctructors 
        ===================================
        */

        /**
        * @brief Default constructor for the Quaternion object.
        */
        Quaternion();
        
        /**
        * @brief Constructs a Quaternion object with the given w, x, y, and z values.
        * @param w The scalar component of the quaternion.
        * @param x The 1st vector component the quaternion.
        * @param y The 2nd vector component the quaternion.
        * @param z The 3rd vector component the quaternion.
        * @param normalize Whether to normalize the quaternion.
        */
        Quaternion(double w, double x, double y, double z, bool normalize = true) {
            _w = w;
            _x = x;
            _y = y;
            _z = z;
            if (normalize)
            {
                this->normalize();
            }
        };

        /**
        * @brief Destructor.
        */
        ~Quaternion() {};

        /* 
        ===================
             Operators 
        ===================
        */

        /**
        * @brief Operator for element-wise multiplying the quaternion by a scalar.
        * @param c The scalar to multiply by.
        * @return The quaternion multiplied by the scalar.
        */
        Quaternion operator*(double c) const { return Quaternion(_w*c, _x*c, _y*c, _z*c); };
        
        /**
        * @brief Operator for element-wise dividing the quaternion by a scalar.
        * @param c The scalar to divide by.
        * @return The quaternion divided by the scalar.
        */
        Quaternion operator/(double c) const { return Quaternion(_w/c, _x/c, _y/c, _z/c); };
        
        /**
        * @brief Operator for checking equality with another quaternion.
        * @param q The quaternion to check equality with.
        * @return Whether the two quaternions are equal within this->epsilon().
        */
        bool operator==(Quaternion q) const { 
            return std::abs(_w - q[0]) <= _epsilon 
                    && std::abs(_x - q[1]) <= _epsilon 
                    && std::abs(_y - q[2]) <= _epsilon
                    && std::abs(_z - q[3]) <= _epsilon;
        }; 

        /**
        * @brief Operator for multiplying two quaternions via their Hamilton product.
        * @param q The quaternion to multiply by.
        * @return The Hamilton product of the two quaternions.
        */
        Quaternion operator*(const Quaternion& q) const {
            return Quaternion(
                _w*q[0] - _x*q[1] - _y*q[2] - _z*q[3],
                _w*q[1] + _x*q[0] + _y*q[3] - _z*q[2],
                _w*q[2] - _x*q[3] + _y*q[0] + _z*q[1],
                _w*q[3] + _x*q[2] - _y*q[1] + _z*q[0]
            );
        }

        /**
        * @brief Operator for rotating a vector via this quaternion via PASSIVE rotation operator.
        * @param v The vector to rotate.
        * @return The vector rotated by this quaternion via a PASSIVE rotation.
        */
        Vector3 operator*(const Vector3& v) const { 
            // passive rotation
            // TODO check this
            Quaternion p = Quaternion(0.0, v[0], v[1], v[2]);
            Quaternion pp = *this * p * this->conjugate();
            return Vector3(pp[1], pp[2], pp[3]);
        };

        /**
        * @brief Operator for accessing the w, x, y, and z values of the quaternion.
        * @param idx The index of the value to access.
        * @return The value at the given index.
        */
        double operator[](int idx) const {
            if (idx == 0)
            {
                return _w;
            }
            if (idx == 1)
            {
                return _x;
            }
            if (idx == 2)
            {
                return _y;
            }
            if (idx == 3)
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
        * @brief Gets pointer to the w value of the quaternion.
        * @return Pointer to the w value of the quaternion.
        */
        double* w() { return &_w; };
        
        /**
        * @brief Gets pointer to the x value of the quaternion.
        * @return Pointer to the x value of the quaternion.
        */
        double* x() { return &_x; };
        
        /**
        * @brief Gets pointer to the y value of the quaternion.
        * @return Pointer to the y value of the quaternion.
        */
        double* y() { return &_y; };

        /** 
        * @brief Gets pointer to the z value of the quaternion.
        * @return Pointer to the z value of the quaternion.
        */
        double* z() { return &_z; };

        /**
        * @brief Normalizes the quaternion.
        */
        void normalize() { 
            if (!isUnit())
            {
                double n = norm();
                _w /= n;
                _x /= n;
                _y /= n;
                _z /= n;
            }
        }

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
        * @brief Checks if the quaternion is a unit quaternion.
        * @return Whether the quaternion is a unit quaternion.
        */
        bool isUnit() const { return std::abs(norm() - 1.0) <= _epsilon; };

        /**
        * @brief Returns the conjugate of the quaternion.
        * @return The conjugate of the quaternion.
        */
        Quaternion conjugate() const { return Quaternion(_w, -_x, -_y, -_z); };
        
        /**
        * @brief Returns the inverse of the quaternion.
        * @return The inverse of the quaternion.
        */
        Quaternion inverse() const { return conjugate() / pow(norm(), 2); };

        /**
        * @brief Returns the norm of the quaternion.
        * @return The norm of the quaternion.
        */
        double norm() const { return std::sqrt(pow(_w, 2) + pow(_x, 2) + pow(_y, 2) + pow(_z, 2)); };

        /**
        * @brief Prints the w, x, y, and z values of the quaternion to stdout.
        */
        void print() const {
            std::cout << "w: " << _w << " x: " << _x << " y: " << _y << " z: " << _z << std::endl;
        }

    protected:
        /**
        * @brief The scalar component of the quaternion.
        */
        double _w = 1.0;
        
        /**
        * @brief The 1st vector component of the quaternion.
        */
        double _x = 0.0;
        
        /**
        * @brief The 2nd vector component of the quaternion.
        */
        double _y = 0.0;
        
        /**
        * @brief The 3rd vector component of the quaternion.
        */
        double _z = 0.0;
        
        /**
        * @brief The epsilon value for floating point comparisons.
        */
        double _epsilon = STELL_EPSILON;

    private:
};


/**
* @brief A class representing a 3x3 matrix.
*/
class Matrix33
{    
    public:

        /* 
        ===================================
             Constructors/Desctructors 
        ===================================
        */

        /**
        * @brief Default constructor for the Matrix33 object.
        */
        Matrix33();
        
        /**
        * @brief Constructs a Matrix33 object with the given x, y, and z vectors.
        * @param x The 1st row of the matrix.
        * @param y The 2nd row of the matrix.
        * @param z The 3rd row of the matrix.
        */
        Matrix33(Vector3 x, Vector3 y, Vector3 z) : _x(x), _y(y), _z(z) {};
        
        /**
        * @brief Constructs a Matrix33 object with the given elements.
        * @param x1 The 1st element of the 1st row.
        * @param x2 The 2nd element of the 1st row.
        * @param x3 The 3rd element of the 1st row.
        * @param y1 The 1st element of the 2nd row.
        * @param y2 The 2nd element of the 2nd row.
        * @param y3 The 3rd element of the 2nd row.
        * @param z1 The 1st element of the 3rd row.
        * @param z2 The 2nd element of the 3rd row.
        * @param z3 The 3rd element of the 3rd row.
        */
        Matrix33(double x1, double x2, double x3, double y1, double y2, double y3, double z1, double z2, double z3) : _x(x1, x2, x3), _y(y1, y2, y3), _z(z1, z2, z3) {};
        
        /**
        * @brief Destructor
        */
        ~Matrix33() {};

        /* 
        ===================
             Operators 
        ===================
        */

        /**
        * @brief Operator for left-multiplying a vector by the matrix.
        * @param v The vector to multiply by.
        * @return The vector multiplied by the matrix.
        */
        Vector3 operator*(Vector3 v) const { 
            return Vector3(
                _x.dot(v),
                _y.dot(v),
                _z.dot(v)
            );
        };

        /**
        * @brief Operator for element-wise multiplying the matrix by a scalar.
        * @param c The scalar to multiply by.
        * @return The matrix multiplied element-wise by the scalar.
        */
        Matrix33 operator*(double c) const {
            return Matrix33(
                _x * c,
                _y * c,
                _z * c
            );
        };

        /**
        * @brief Operator for element-wise dividing the matrix by a scalar.
        * @param c The scalar to divide by.
        * @return The matrix divided element-wise by the scalar.
        */
        Matrix33 operator/(double c) const {
            return Matrix33(
                _x / c,
                _y / c,
                _z / c
            );
        };

        /**
        * @brief Operator for accessing the 1st, 2nd, and 3rd rows of the matrix.
        * @param idx The index of the row to access.
        * @return The row at the given index.
        */
        Vector3 operator[](int idx) const {
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
        * @brief Computes the determinant of the matrix.
        * @return The determinant of the matrix.
        */
        double det() const { 
            return _x[0]*(_y[1]*_z[2] - _y[2]*_z[1]) 
                   - _x[1]*(_y[0]*_z[2] - _y[2]*_z[0]) 
                   + _x[2]*(_y[0]*_z[1] - _y[1]*_z[0]); 
        };

        /**
        * @brief Computes the transpose of the matrix.
        * @return The transpose of the matrix.
        */
        Matrix33 transpose() const {
            return Matrix33(
                _x[0], _y[0], _z[0],
                _x[1], _y[1], _z[1],
                _x[2], _y[2], _z[2]
            );
        };

        /**
        * @brief Computes the adjoint of the matrix.
        * @return The adjoint of the matrix.
        */
        Matrix33 adjoint() const {
            return Matrix33(
                _y[1]*_z[2] - _y[2]*_z[1],
                _x[2]*_z[1] - _x[1]*_z[2],
                _x[1]*_y[2] - _x[2]*_y[1],
                _y[2]*_z[0] - _y[0]*_z[2],
                _x[0]*_z[2] - _x[2]*_z[0],
                _x[2]*_y[0] - _x[0]*_y[2],
                _y[0]*_z[1] - _y[1]*_z[0],
                _x[1]*_z[0] - _x[0]*_z[1],
                _x[0]*_y[1] - _x[1]*_y[0]
            );
        };

        /**
        * @brief Computes the inverse of the matrix.
        * @return The inverse of the matrix.
        */
        Matrix33 inverse() const {
            return adjoint().transpose() / det();
        };


    protected:
        /**
        * @brief The 1st row of the matrix.
        */
        Vector3 _x {0.0, 0.0, 0.0};
        
        /**
        * @brief The 2nd row of the matrix.
        */
        Vector3 _y {0.0, 0.0, 0.0};
        
        /**
        * @brief The 3rd row of the matrix.
        */
        Vector3 _z {0.0, 0.0, 0.0};

    private:
};



} // end namespace Stellarium

#endif // end STELL_MATH