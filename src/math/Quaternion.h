#ifndef STELL_QUATERNION
#define STELL_QUATERNION

#include <cassert>

#include "Constants.h"
#include "Matrix33.h"
#include "Vector3.h"

namespace Stellarium
{

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
        Quaternion() = default;
        
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
        * @brief Constructs a Quaternion based on an axis-angle rotation.
        * @param axis The axis of rotation.
        * @param angle The angle of rotation in radians.
        */
        Quaternion(const Vector3& axis, double angle) {
            Vector3 ax = axis.getNormalized();
            double s = std::sin(angle/2);
            _w = std::cos(angle/2);
            _x = ax[0]*s;
            _y = ax[1]*s;
            _z = ax[2]*s;
            this->normalize();
        };

        /**
        * @brief Constructs a Quaternion based on a roll-pitch-yaw (XYZ) Euler rotation sequence.
        * @param roll The roll Euler angle in radians.
        * @param pitch The pitch Euler angle in radians.
        * @param yaw The yaw Euler angle in radians.
        */
        Quaternion(double roll, double pitch, double yaw) {
            // TODO: this needs to be checked
            Quaternion q_roll = Quaternion(Vector3(1, 0, 0), roll);
            Quaternion q_pitch = Quaternion(Vector3(0, 1, 0), pitch);
            Quaternion q_yaw = Quaternion(Vector3(0, 0, 1), yaw);
            Quaternion q = q_yaw * q_pitch * q_roll;
            _w = q[0];
            _x = q[1];
            _y = q[2];
            _z = q[3];
            this->normalize();
        };


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
        Quaternion operator*(double c) const { return Quaternion(_w*c, _x*c, _y*c, _z*c, false); };
        
        /**
        * @brief Operator for element-wise dividing the quaternion by a scalar.
        * @param c The scalar to divide by.
        * @return The quaternion divided by the scalar.
        */
        Quaternion operator/(double c) const { return Quaternion(_w/c, _x/c, _y/c, _z/c, false); };
        
        /**
        * @brief Operator for checking equality with another quaternion.
        * @param q The quaternion to check equality with.
        * @return Whether the two quaternions are equal within this->epsilon().
        */
        bool operator==(const Quaternion& q) const { 
            return std::abs(_w - q[0]) <= _epsilon 
                    && std::abs(_x - q[1]) <= _epsilon 
                    && std::abs(_y - q[2]) <= _epsilon
                    && std::abs(_z - q[3]) <= _epsilon;
        }; 

        /**
        * @brief Operator for checking equality with a scalar. This implies that the vector portions of the quaternion are zero.
        * @param c The scalar to check equality with.
        * @return Whether the quaternion is equal to the scalar within this->epsilon().
        */
        bool operator==(double c) const { 
            return std::abs(_w - c) <= _epsilon 
                    && std::abs(_x) <= _epsilon 
                    && std::abs(_y) <= _epsilon
                    && std::abs(_z) <= _epsilon;
        };

        /**
        * @brief Operator for multiplying two quaternions via their Hamilton product.
        * @param q The quaternion to multiply by.
        * @return The Hamilton product of the two quaternions.
        */
        Quaternion operator*(const Quaternion& q) const {
            bool normalize = this->isUnit() && q.isUnit();
            return Quaternion(
                _w*q[0] - _x*q[1] - _y*q[2] - _z*q[3],
                _w*q[1] + _x*q[0] + _y*q[3] - _z*q[2],
                _w*q[2] - _x*q[3] + _y*q[0] + _z*q[1],
                _w*q[3] + _x*q[2] - _y*q[1] + _z*q[0],
                normalize
            );
        }

        /**
        * @brief Rotates the vector v. This is an active rotation (i.e. rotates vectors or frames from the starting pose to the end pose).
        * @param v The vector.
        * @return The rotated vector.
        */
        Vector3 operator*(const Vector3& v) const { 
            if (isIdentity())
            {
                return v;
            }
            
            assert(isUnit());

            // Reference: https://faculty.sites.iastate.edu/jia/files/inline-files/quaternion.pdf
            // Theorem 2:
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
                if (n > _epsilon)
                {
                    _w /= n;
                    _x /= n;
                    _y /= n;
                    _z /= n;
                }
            }
        }

        /**
        * @brief Returns a copy of this quaternion that is normalized. Does not modify the existing quaternion.
        * @return The normalized quaternion.
        */
        Quaternion getNormalized() const {
            Quaternion q = *this;
            q.normalize();
            return q;
        }

        /**
        * @brief Gets the rotation matrix representation of the quaternion.
        * This is an active rotation. That is, it actively rotates a vector or frame from the starting pose to the end pose.
        * @return The rotation matrix representation of the quaternion.
        */
        Matrix33 getRotationMatrix() const {

            // See: https://www.mathworks.com/help/nav/ref/quaternion.rotmat.html

            assert(isUnit());

            double a = _w;
            double b = _x;
            double c = _y;
            double d = _z;

            return Matrix33(
                2*a*a - 1 + 2*b*b, 2*b*c - 2*a*d, 2*b*d + 2*a*c,
                2*b*c + 2*a*d, 2*a*a - 1 + 2*c*c, 2*c*d - 2*a*b,
                2*b*d - 2*a*c, 2*c*d + 2*a*b, 2*a*a - 1 + 2*d*d
            );
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
        * @brief Checks if the quaternion is the identity quaternion.
        * @return Whether the quaternion is the identity quaternion.
        */
        bool isIdentity() const { return *this == Quaternion(1.0, 0.0, 0.0, 0.0); };

        /**
        * @brief Returns the conjugate of the quaternion.
        * @return The conjugate of the quaternion.
        */
        Quaternion conjugate() const { return Quaternion(_w, -_x, -_y, -_z, false); };
        
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

        Vector3 getRollPitchYaw() const { return this->getRotationMatrix().getRollPitchYaw(); };

        /**
        * @brief Prints the w, x, y, and z values of the quaternion to stdout.
        */
        void print() const {
            std::cout << "w: " << _w << " x: " << _x << " y: " << _y << " z: " << _z << std::endl;
        }

    protected:
    
    private:
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
};


/**
* @brief Global operator for element-wise multiplying a quaternion by a scalar on the lefthand side.
* @param c The scalar to multiply by.
* @param q The quaternion to multiply by.
* @return The quaternion multiplied element-wise by the scalar.
*/
inline Quaternion operator*(double c, const Quaternion& q)
{
    return q*c;
};


} // end namespace Stellarium

#endif // end STELL_QUATERNION

