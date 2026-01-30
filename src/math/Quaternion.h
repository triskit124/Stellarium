#pragma once

#include <cassert>

#include "RotationMatrix.h"
#include "Vector3.h"

namespace Stellarium
{

/**
* @brief A class representing a quaternion.
*/
class Quaternion: public MathBase
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
            this->w = w;
            this->x = x;
            this->y = y;
            this->z = z;
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
            this->w = std::cos(angle/2);
            this->x = ax[0]*s;
            this->y = ax[1]*s;
            this->z = ax[2]*s;
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
            Quaternion qyaw = Quaternion(Vector3(0, 0, 1), yaw);
            Quaternion q = qyaw * q_pitch * q_roll;
            this->w = q.w;
            this->x = q.x;
            this->y = q.y;
            this->z = q.z;
            this->normalize();
        };

        /*
        ====================
            Data Members
        ====================
        */

        /**
        * @brief The scalar component of the quaternion.
        */
        double w = 1.0;

        /**
        * @brief The 1st vector component of the quaternion.
        */
        double x = 0.0;

        /**
        * @brief The 2nd vector component of the quaternion.
        */
        double y = 0.0;

        /**
        * @brief The 3rd vector component of the quaternion.
        */
        double z = 0.0;

        /*
        ===================
             Operators
        ===================
        */

        Quaternion operator*(double c) const { return Quaternion(w*c, x*c, y*c, z*c, false); };
        Quaternion operator/(double c) const { return Quaternion(w/c, x/c, y/c, z/c, false); };

        bool operator==(const Quaternion& q) const {
            return std::abs(w - q.w) <= _epsilon
                    && std::abs(x - q.x) <= _epsilon
                    && std::abs(y - q.y) <= _epsilon
                    && std::abs(z - q.z) <= _epsilon;
        };

        bool operator==(double c) const {
            return std::abs(w - c) <= _epsilon
                    && std::abs(x) <= _epsilon
                    && std::abs(y) <= _epsilon
                    && std::abs(z) <= _epsilon;
        };

        // Hamiltom product
        Quaternion operator*(const Quaternion& q) const {
            bool normalize = this->isUnit() && q.isUnit();
            return Quaternion(
                w*q.w - x*q.x - y*q.y - z*q.z,
                w*q.x + x*q.w + y*q.z - z*q.y,
                w*q.y - x*q.z + y*q.w + z*q.x,
                w*q.z + x*q.y - y*q.x + z*q.w,
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
            Quaternion pp = *this * p * this->getConjugate();

            return Vector3(pp.x, pp.y, pp.z);
        };

        /**
        * @brief Operator for accessing the w, x, y, and z values of the quaternion.
        * @param idx The index of the value to access.
        * @return The value at the given index.
        */
        double operator[](int idx) const {
            if (idx == 0)
            {
                return w;
            }
            if (idx == 1)
            {
                return x;
            }
            if (idx == 2)
            {
                return y;
            }
            if (idx == 3)
            {
                return z;
            }
            throw std::invalid_argument("invalid index");
        }

        /**
        * @brief Operator for accessing the w, x, y, and z values of the quaternion.
        * @param idx The index of the value to access.
        * @return The value at the given index.
        */
        double& operator[](int idx) {
            if (idx == 0)
            {
                return w;
            }
            if (idx == 1)
            {
                return x;
            }
            if (idx == 2)
            {
                return y;
            }
            if (idx == 3)
            {
                return z;
            }
            throw std::invalid_argument("invalid index");
        }

        /*
        ===================
              Methods
        ===================
        */

        /**
        * @brief Normalizes the quaternion.
        */
        void normalize() {
            if (!isUnit())
            {
                double n = getNorm();
                if (n > _epsilon)
                {
                    this->w /= n;
                    this->x /= n;
                    this->y /= n;
                    this->z /= n;
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
        RotationMatrix getRotationMatrix() const {

            // See: https://www.mathworks.com/help/nav/ref/quaternion.rotmat.html

            assert(isUnit());

            double a = this->w;
            double b = this->x;
            double c = this->y;
            double d = this->z;

            return RotationMatrix(
                Vector3(2*a*a - 1 + 2*b*b, 2*b*c - 2*a*d, 2*b*d + 2*a*c),
                Vector3(2*b*c + 2*a*d, 2*a*a - 1 + 2*c*c, 2*c*d - 2*a*b),
                Vector3(2*b*d - 2*a*c, 2*c*d + 2*a*b, 2*a*a - 1 + 2*d*d)
            );
        }

        /**
        * @brief Checks if the quaternion is a unit quaternion.
        * @return Whether the quaternion is a unit quaternion.
        */
        bool isUnit() const { return std::abs(getNorm() - 1.0) <= _epsilon; };

        /**
        * @brief Checks if the quaternion is the identity quaternion.
        * @return Whether the quaternion is the identity quaternion.
        */
        bool isIdentity() const { return *this == Quaternion(1.0, 0.0, 0.0, 0.0); };

        /**
        * @brief Returns the conjugate of the quaternion.
        * @return The conjugate of the quaternion.
        */
        Quaternion getConjugate() const { return Quaternion(w, -x, -y, -z, false); };

        /**
        * @brief Returns the inverse of the quaternion.
        * @return The inverse of the quaternion.
        */
        Quaternion getInverse() const { return getConjugate() / pow(getNorm(), 2); };

        /**
        * @brief Returns the norm of the quaternion.
        * @return The norm of the quaternion.
        */
        double getNorm() const { return std::sqrt(pow(w, 2) + pow(x, 2) + pow(y, 2) + pow(z, 2)); };

        Vector3 getRollPitchYaw() const { return this->getRotationMatrix().getRollPitchYaw(); };

        /**
        * @brief Prints the w, x, y, and z values of the quaternion to stdout.
        */
        void print(const std::string& s) const {
            std::cout << "s: " << s << "w: " << w << " x: " << x << " y: " << y << " z: " << z << std::endl;
        }

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
