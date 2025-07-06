#ifndef STELL_MATRIX44_H
#define STELL_MATRIX44_H

#include <array>
#include <cmath>
#include <cstddef>
#include <stdexcept>

#include "Matrix33.h"
#include "Quaternion.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Stellarium
{

/**
* @brief A class representing a 4x4 Homogeneous Transformation Matrix.
*/
class Matrix44
{    
    public:

        /* 
        ===================================
             Constructors/Destructors 
        ===================================
        */

        /**
        * @brief Default constructor for the Matrix44 object.
        */
        Matrix44(Quaternion quat = Quaternion(), Vector3 translation = Vector3()) {
            setRotation(quat.getRotationMatrix());
            setTranslation(translation);
        };
        
        /**
        * @brief Constructs a Matrix44 object with the given x, y, z, and w vectors.
        * @param x The 1st row of the matrix.
        * @param y The 2nd row of the matrix.
        * @param z The 3rd row of the matrix.
        * @param w The 4th row of the matrix.
        */
        Matrix44(Vector4 x, Vector4 y, Vector4 z, Vector4 w) : x(x), y(y), z(z), w(w) {};
        

        /**
        * @brief Construct from a rotation matrix and translation vector.
        * The resulting homogeneous transformation matrix will represent a rotation and translation of a vector/frame from the initial pose to the final pose. 
        * The translation and rotation are both w.r.t the initial pose, expressed in the initial pose.
        * @param rotation The rotation matrix.
        * @param translation The translation vector.
        */
        Matrix44(Matrix33 rotation, Vector3 translation) {
            setRotation(rotation);
            setTranslation(translation);
        };
        
        /*
        ====================
            Data Members
        ====================
        */
        
        /**
        * @brief The 1st row of the matrix.
        */
        Vector4 x {1.0, 0.0, 0.0, 0.0};
        
        /**
        * @brief The 2nd row of the matrix.
        */
        Vector4 y {0.0, 1.0, 0.0, 0.0};
        
        /**
        * @brief The 3rd row of the matrix.
        */
        Vector4 z {0.0, 0.0, 1.0, 0.0};

        /**
        * @brief The 4th row of the matrix.
        */
        Vector4 w {0.0, 0.0, 0.0, 1.0};

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
        Vector4 operator*(Vector4 v) const { 
            return Vector4(
                x.dot(v),
                y.dot(v),
                z.dot(v),
                w.dot(v)
            );
        };

        /**
        * @brief Operator for left-multiplying a matrix by this matrix.
        * @param m The matrix to multiply by.
        * @return The matrix left-multiplied by this matrix.
        */
        Matrix44 operator*(const Matrix44& m) const {
            Matrix44 mat = m.getTranspose();
            return Matrix44(
                *this * mat[0],
                *this * mat[1],
                *this * mat[2],
                *this * mat[3]
            ).getTranspose();
        };

        /**
        * @brief Operator for tranforming a Vector3 by this matrix.
        * @param v The vector to multiply by.
        * @return The vector left-multiplied by the matrix.
        */
        Vector3 operator*(const Vector3& v) const {
            return transform(v);
        };

        /**
        * @brief Operator for element-wise multiplying the matrix by a scalar.
        * @param c The scalar to multiply by.
        * @return The matrix multiplied element-wise by the scalar.
        */
        Matrix44 operator*(double c) const {
            return Matrix44(
                x * c,
                y * c,
                z * c,
                w * c
            );
        };

        /**
        * @brief Operator for element-wise dividing the matrix by a scalar.
        * @param c The scalar to divide by.
        * @return The matrix divided element-wise by the scalar.
        */
        Matrix44 operator/(double c) const {
            return Matrix44(
                x / c,
                y / c,
                z / c,
                w / c
            );
        };

        /**
        * @brief Operator for accessing the rows of the matrix.
        * @param idx The index of the row to access.
        * @return The row at the given index.
        */
        Vector4 operator[](int idx) const {
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
            throw std::invalid_argument("invalid index"); 
        }

        /**
        * @brief Operator for accessing the rows of the matrix.
        * @param idx The index of the row to access.
        * @return The row at the given index.
        */
        Vector4& operator[](int idx) {
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
            throw std::invalid_argument("invalid index"); 
        }

        /**
        * @brief Operator testing equality of two matrices.
        * @param m The matrix to compare to.
        * @return True if the matrices are equal, false otherwise.
        */
        bool operator==(const Matrix44& m) const {
            return x == m[0] && y == m[1] && z == m[2] && w == m[3];
        };

        /* 
        ===================
              Methods 
        ===================
        */

        /**
        * @brief Gets the rotation matrix from the homogeneous transformation matrix.
        * @return The rotation matrix.
        */
        Matrix33 getRotation() const {
            return Matrix33(
                Vector3(x[0], x[1], x[2]),
                Vector3(y[0], y[1], y[2]),
                Vector3(z[0], z[1], z[2])
            );
        };

        /**
        * @brief Gets the translation vector from the homogeneous transformation matrix.
        * @return The translation vector.
        */
        Vector3 getTranslation() const {
            return Vector3(x[3], y[3], z[3]);
        };

        /**
        * @brief Sets the rotation matrix of the homogeneous transformation matrix.
        * @param rotation The new rotation matrix.
        */
        void setRotation(Matrix33 rotation) {
            x = Vector4(rotation[0][0], rotation[0][1], rotation[0][2], x[3]);
            y = Vector4(rotation[1][0], rotation[1][1], rotation[1][2], y[3]);
            z = Vector4(rotation[2][0], rotation[2][1], rotation[2][2], z[3]);
        };

        /**
        * @brief Sets the translation vector of the homogeneous transformation matrix.
        * @param translation The new translation vector.
        */
        void setTranslation(Vector3 translation) {
            x = Vector4(x[0], x[1], x[2], translation[0]);
            y = Vector4(y[0], y[1], y[2], translation[1]);
            z = Vector4(z[0], z[1], z[2], translation[2]);
        };

        /**
        * @brief Computes the transpose of the matrix.
        * @return The transpose of the matrix.
        */
        Matrix44 getTranspose() const {
            return Matrix44(
                Vector4(x[0], y[0], z[0], w[0]),
                Vector4(x[1], y[1], z[1], w[1]),
                Vector4(x[2], y[2], z[2], w[2]),
                Vector4(x[3], y[3], z[3], w[3])
            );
        };

        /**
        * @brief Transforms a Vector3 by the homogeneous transform matrix.
        * @param v The vector to transform.
        * @return The transformed vector.
        */
        Vector3 transform(const Vector3& v) const {
            return (getRotation() * v) + getTranslation();
        };

        /**
        * @brief Returns the inverse of the transform represented by the matrix.
        * @return The inverse transform.
        */
        Matrix44 getInverseTransform() const {
            return Matrix44(getRotation().inverse(), -(getRotation().inverse() * getTranslation()));
        };

        std::array<float, 16> getColMajorArray() const {
            Matrix44 mat = getTranspose();
            std::array<float, 16> array;
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    array[4*i + j] = static_cast<float>(mat[i][j]);
                }
            }
            return array;
        }

        /**
        * @brief Prints the matrix to stdout.
        */
        void print() const {
            x.print();
            y.print();
            z.print();
            w.print();
        };

};

/**
* @brief Global operator for element-wise multiplying a matrix by a scalar on the lefthand side.
* @param c The scalar to multiply by.
* @param m The matrix to multiply by.
* @return The matrix multiplied element-wise by the scalar.
*/
inline Matrix44 operator*(double c, const Matrix44& m)
{
    return m*c;
};

/**
* @brief Global operator for element-wise dividing a matrix by a scalar on the lefthand side.
* @param c The scalar to divide by.
* @param m The matrix to divide.
* @return The matrix divided element-wise by the scalar.
*/
inline Matrix44 operator/(double c, const Matrix44& m)
{
    return m/c;
};


} // end namespace Stellarium

#endif // end STELL_MATRIX44_H