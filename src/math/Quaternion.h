#pragma once

#include <cassert>

#include "RotationMatrix.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Stellarium
{

/**
* @brief A class representing a quaternion.
* Stored as a Vector4(w, x, y, z) where w is the scalar component and (x, y, z) is the vector component.
*/
class Quaternion: public Vector4
{
    public:

        /*
        ===================================
             Constructors/Desctructors
        ===================================
        */

        /**
        * @brief Default constructor for the Quaternion object. Initializes to identity (1, 0, 0, 0).
        */
        Quaternion() : Vector4(1.0, 0.0, 0.0, 0.0) {}

        /**
        * @brief Constructs a Quaternion object with the given w, x, y, and z values.
        * @param w The scalar component of the quaternion.
        * @param x The 1st vector component the quaternion.
        * @param y The 2nd vector component the quaternion.
        * @param z The 3rd vector component the quaternion.
        * @param normalize Whether to normalize the quaternion.
        */
        Quaternion(double w, double x, double y, double z, bool normalize = true) : Vector4(w, x, y, z) {
            if (normalize)
            {
                this->normalize();
            }
        };

        /**
        * @brief Converting constructor from base Vector4.
        * @param v The Vector4 to convert from.
        * @param normalize Whether to normalize the quaternion.
        */
        explicit Quaternion(const Vector4& v, bool normalize = true) : Vector4(v) {
            if (normalize)
            {
                this->normalize();
            }
        }

        /**
        * @brief Converting constructor from base Vector.
        * @param v The Vector to convert from.
        * @param normalize Whether to normalize the quaternion.
        */
        explicit Quaternion(const Vector& v, bool normalize = true) : Vector4(v) {
            if (normalize)
            {
                this->normalize();
            }
        }

        /**
        * @brief Constructs a Quaternion based on an axis-angle rotation. Represents a frame rotated by +angle about axis
        * (passive rotation): `q * v` gives the components of v in the rotated frame.
        * @param axis The axis of rotation.
        * @param angle The angle of rotation in radians.
        */
        Quaternion(const Vector3& axis, double angle) {
            Vector3 ax = axis.getNormalized();
            double s = std::sin(angle/2);
            (*this)[0] = std::cos(angle/2);
            (*this)[1] = ax[0]*s;
            (*this)[2] = ax[1]*s;
            (*this)[3] = ax[2]*s;
            this->normalize();
        };

        /**
        * @brief Constructs a Quaternion based on a roll-pitch-yaw Euler rotation sequence (body 1-2-3: the frame is rotated
        * by roll about x, then by pitch about the new y, then by yaw about the new z).
        * @param roll The roll Euler angle in radians.
        * @param pitch The pitch Euler angle in radians.
        * @param yaw The yaw Euler angle in radians.
        */
        Quaternion(double roll, double pitch, double yaw) {
            Quaternion q_roll = Quaternion(Vector3(1, 0, 0), roll);
            Quaternion q_pitch = Quaternion(Vector3(0, 1, 0), pitch);
            Quaternion qyaw = Quaternion(Vector3(0, 0, 1), yaw);
            // Passive quaternions compose left to right: q1 * q2 applies the q1 frame rotation first.
            Quaternion q = q_roll * q_pitch * qyaw;
            (*this)[0] = q[0];
            (*this)[1] = q[1];
            (*this)[2] = q[2];
            (*this)[3] = q[3];
            this->normalize();
        };



        /*
        ===================
             Operators
        ===================
        */

        Quaternion operator*(double c) const { return Quaternion(Vector4::operator*(c), false); };
        Quaternion operator/(double c) const { return Quaternion(Vector4::operator/(c), false); };

        bool operator==(const Quaternion& q) const {
            return Vector4::operator==(q);
        };

        bool operator==(double c) const {
            return std::abs(getScalarPart() - c) <= _epsilon && getVectorPart().getNorm() <= _epsilon;
        };

        // Hamilton product
        Quaternion operator*(const Quaternion& q) const {
            bool do_normalize = this->isUnit() && q.isUnit();
            double w = (*this)[0], x = (*this)[1], y = (*this)[2], z = (*this)[3];
            return Quaternion(
                w*q[0] - x*q[1] - y*q[2] - z*q[3],
                w*q[1] + x*q[0] + y*q[3] - z*q[2],
                w*q[2] - x*q[3] + y*q[0] + z*q[1],
                w*q[3] + x*q[2] - y*q[1] + z*q[0],
                do_normalize
            );
        }

        /**
        * @brief Applies a passive rotation to v: gives the components of v in the rotated frame.
        * @param v The vector, expressed in the original frame.
        * @return The same vector, expressed in the rotated frame.
        */
        Vector3 operator*(const Vector3& v) const {
            if (isIdentity())
            {
                return v;
            }

            if (!isUnit())
            {
                throw std::runtime_error("Quaternion must be a unit quaternion to rotate a vector.");
            }

            // Reference: https://faculty.sites.iastate.edu/jia/files/inline-files/quaternion.pdf
            // Theorem 2: q_conj * v * q rotates the coordinate frame by theta about the axis while v is fixed.
            Quaternion p = Quaternion(0.0, v[0], v[1], v[2], false);
            Quaternion pp = this->getConjugate() * p * *this;

            return pp.getVectorPart();
        };

        /*
        ===================
              Methods
        ===================
        */

        /**
        * @brief Gets the rotation matrix representation of the quaternion.
        * This is a passive rotation (direction cosine matrix) meaning it
        * transforms a vector from the initial frame to the final frame assuming
        * that the frame itself rotates while the vector remains stationary.
        * @return The rotation matrix representation of the quaternion.
        */
        RotationMatrix getRotationMatrix() const {

            // See: https://www.mathworks.com/help/nav/ref/quaternion.rotmat.html ('frame' rotation type)

            if (isIdentity())
            {
                return RotationMatrix();
            }

            if (!isUnit())
            {
                throw std::runtime_error("Quaternion must be a unit quaternion to get a rotation matrix.");
            }

            double a = (*this)[0];
            double b = (*this)[1];
            double c = (*this)[2];
            double d = (*this)[3];

            return RotationMatrix(
                Vector3(2*a*a - 1 + 2*b*b, 2*b*c + 2*a*d, 2*b*d - 2*a*c),
                Vector3(2*b*c - 2*a*d, 2*a*a - 1 + 2*c*c, 2*c*d + 2*a*b),
                Vector3(2*b*d + 2*a*c, 2*c*d - 2*a*b, 2*a*a - 1 + 2*d*d)
            );
        }

        /**
        * @brief Checks if the quaternion is the identity quaternion.
        * @return Whether the quaternion is the identity quaternion.
        */
        bool isIdentity() const { return *this == Quaternion(); };

        /**
        * @brief Returns the conjugate of the quaternion.
        * @return The conjugate of the quaternion.
        */
        Quaternion getConjugate() const { return Quaternion((*this)[0], -(*this)[1], -(*this)[2], -(*this)[3], false); };

        /**
        * @brief Returns the inverse of the quaternion.
        * @return The inverse of the quaternion.
        */
        Quaternion getInverse() const { return getConjugate() / pow(getNorm(), 2); };

        /**
        * @brief Returns the Roll-Pitch-Yaw Euler angle sequence for the rotation encoded by the quaternion.
        * @return The Roll-Pitch-Yaw Euler angle sequence.
        */
        Vector3 getRollPitchYaw() const { return this->getRotationMatrix().getRollPitchYaw(); };

        /**
        * @brief Returns the scalar portion (w) of the quaternion.
        * @return The scalar portion of the quaternion.
        */
        double getScalarPart() const { return (*this)[0]; };

        /**
        * @brief Returns the vector portion (x, y, z) of the quaternion.
        * @return The vector portion of the quaternion.
        */
        Vector3 getVectorPart() const { return Vector3((*this)[1], (*this)[2], (*this)[3]); };

        /**
        * @brief Returns a normalized copy of the quaternion
        * @return The normalized quaternion.
        */
        Quaternion getNormalized() const { return Quaternion(Vector4::getNormalized(), false); };

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
