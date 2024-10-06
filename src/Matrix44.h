#ifndef STELL_MATRIX44
#define STELL_MATRIX44

#include <cmath>
#include <stdexcept>

#include "Constants.h"
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
        Matrix44();
        
        /**
        * @brief Constructs a Matrix44 object with the given x, y, z, and w vectors.
        * @param x The 1st row of the matrix.
        * @param y The 2nd row of the matrix.
        * @param z The 3rd row of the matrix.
        * @param w The 4th row of the matrix.
        */
        Matrix44(Vector4 x, Vector4 y, Vector4 z, Vector4 w) : _x(x), _y(y), _z(z), _w(w) {};
        

        /**
        * @brief Construct from a rotation matrix and translation vector.
        * The resulting homogeneous transformation matrix will represent a rotation and translation of a vector/frame from the initial pose to the final pose. 
        * The translation and rotation are both w.r.t the initial pose, expressed in the initial pose.
        * @param rotation The rotation matrix.
        * @param translation The translation vector.
        */
        Matrix44(Matrix33 rotation = Matrix33(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1)), Vector3 translation = Vector3(0, 0, 0)) {
            setRotation(rotation);
            setTranslation(translation);
        };

        /**
        * @brief Construct from a quaternion and translation vector.
        * The resulting homogeneous transformation matrix will represent a rotation and translation of a vector/frame from the initial pose to the final pose. 
        * The translation and rotation are both w.r.t the initial pose, expressed in the initial pose.
        * @param quat The quaternion.
        * @param translation The translation vector.
        */
        Matrix44(Quaternion quat = Quaternion(1, 0, 0, 0), Vector3 translation = Vector3(0, 0, 0)) {
            setRotation(quat.getRotationMatrix());
            setTranslation(translation);
        };

        /**
        * @brief Constructs a perspective projection matrix.
        * See: http://www.songho.ca/opengl/gl_projectionmatrix.html
        * @param fov The field of view in degrees.
        * @param aspect_ratio The aspect ratio of the screen (width / height).
        * @param near Near clipping plane distance.
        * @param far Far clipping plane distance.
        */
        Matrix44(double fov, double aspect_ratio, double near, double far) {
            double tangent = tan((fov * M_PI / 180) / 2.0);
            double top = near * tangent;
            double right = top * aspect_ratio;
            _x = Vector4(near / right, 0.0, 0.0, 0.0);
            _y = Vector4(0.0, near / top, 0.0, 0.0);
            _z = Vector4(0.0, 0.0, -(far + near) / (far - near), -(2 * far * near) / (far - near));
            _w = Vector4(0.0, 0.0, -1.0, 0.0);
        };

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
                _x.dot(v),
                _y.dot(v),
                _z.dot(v),
                _w.dot(v)
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
                _x * c,
                _y * c,
                _z * c,
                _w * c
            );
        };

        /**
        * @brief Operator for element-wise dividing the matrix by a scalar.
        * @param c The scalar to divide by.
        * @return The matrix divided element-wise by the scalar.
        */
        Matrix44 operator/(double c) const {
            return Matrix44(
                _x / c,
                _y / c,
                _z / c,
                _w / c
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

        /**
        * @brief Operator testing equality of two matrices.
        * @param m The matrix to compare to.
        * @return True if the matrices are equal, false otherwise.
        */
        bool operator==(const Matrix44& m) const {
            return _x == m[0] && _y == m[1] && _z == m[2] && _w == m[3];
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
                _x[0], _x[1], _x[2],
                _y[0], _y[1], _y[2],
                _z[0], _z[1], _z[2]
            );
        };

        /**
        * @brief Gets the translation vector from the homogeneous transformation matrix.
        * @return The translation vector.
        */
        Vector3 getTranslation() const {
            return Vector3(_x[3], _y[3], _z[3]);
        };

        /**
        * @brief Sets the rotation matrix of the homogeneous transformation matrix.
        * @param rotation The new rotation matrix.
        */
        void setRotation(Matrix33 rotation) {
            _x = Vector4(rotation[0][0], rotation[0][1], rotation[0][2], _x[3]);
            _y = Vector4(rotation[1][0], rotation[1][1], rotation[1][2], _y[3]);
            _z = Vector4(rotation[2][0], rotation[2][1], rotation[2][2], _z[3]);
        };

        /**
        * @brief Sets the translation vector of the homogeneous transformation matrix.
        * @param translation The new translation vector.
        */
        void setTranslation(Vector3 translation) {
            _x = Vector4(_x[0], _x[1], _x[2], translation[0]);
            _y = Vector4(_y[0], _y[1], _y[2], translation[1]);
            _z = Vector4(_z[0], _z[1], _z[2], translation[2]);
        };

        /**
        * @brief Computes the transpose of the matrix.
        * @return The transpose of the matrix.
        */
        Matrix44 getTranspose() const {
            return Matrix44(
                Vector4(_x[0], _y[0], _z[0], _w[0]),
                Vector4(_x[1], _y[1], _z[1], _w[1]),
                Vector4(_x[2], _y[2], _z[2], _w[2]),
                Vector4(_x[3], _y[3], _z[3], _w[3])
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
        Matrix44 inverseTransform() const {
            return Matrix44(getRotation().inverse(), -(getRotation().inverse() * getTranslation()));
        };

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
        * @brief Prints the matrix to stdout.
        */
        void print() const {
            _x.print();
            _y.print();
            _z.print();
            _w.print();
        };

    protected:

    private:
        /**
        * @brief The 1st row of the matrix.
        */
        Vector4 _x {1.0, 0.0, 0.0, 0.0};
        
        /**
        * @brief The 2nd row of the matrix.
        */
        Vector4 _y {0.0, 1.0, 0.0, 0.0};
        
        /**
        * @brief The 3rd row of the matrix.
        */
        Vector4 _z {0.0, 0.0, 1.0, 0.0};

        /**
        * @brief The 4th row of the matrix.
        */
        Vector4 _w {0.0, 0.0, 0.0, 1.0};

        /**
        * @brief The epsilon value for floating point comparisons.
        */
        double _epsilon = STELL_EPSILON;
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

#endif // end STELL_MATRIX44