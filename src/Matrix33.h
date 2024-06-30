#ifndef STELL_MATRIX33
#define STELL_MATRIX33

#include <stdexcept>

#include "Constants.h"
#include "Vector3.h"

namespace Stellarium
{

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

/**
* @brief Global operator for element-wise multiplying a matrix by a scalar on the lefthand side.
* @param c The scalar to multiply by.
* @param m The matrix to multiply by.
* @return The matrix multiplied element-wise by the scalar.
*/
inline Matrix33 operator*(double c, const Matrix33& m)
{
    return m*c;
};

/**
* @brief Global operator for element-wise dividing a matrix by a scalar on the lefthand side.
* @param c The scalar to divide by.
* @param m The matrix to divide.
* @return The matrix divided element-wise by the scalar.
*/
inline Matrix33 operator/(double c, const Matrix33& m)
{
    return m/c;
};


} // end namespace Stellarium

#endif // end STELL_MATRIX33