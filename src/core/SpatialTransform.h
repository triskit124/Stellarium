#pragma once

#include "Matrix.h"
#include "Matrix33.h"
#include "Quaternion.h"
#include "SpatialInertia.h"
#include "SpatialVector.h"
#include "Vector.h"
#include "Vector3.h"

namespace Stellarium
{

/**
* @brief A class representing a spatial (Plucker) coordinate transform.
* See: Featherstone, Rigid Body Dynamics Algorithms, 2008, pp. 22
*
* A SpatialTransform maps quantities expressed in frame A into  frame B. It is
* stored as the pair (E, r) of Featherstone eq. 2.24/2.25:
*
*   - `r` is the position of B's origin, expressed in A coordinates.
*   - `E` is the 3x3 rotation matrix that tranforms 3D vectors from A to B coordinates
*
* E is stored internally as a `Quaternion`, using the convention that `_quaternion * v` (an active rotation, see
* Quaternion::operator*) evaluates E * v. Consequently, if B's attitude quaternion is `att` in the
* sense used by Frame then `_quaternion == att.getInverse()`.
*
* Concretely, a revolute joint rotating the child by +theta about `k` relative to the parent has
* `_quaternion = Quaternion(k, theta).getInverse()` -- see PinJoint::getJointTransform.
*/
class SpatialTransform
{   
    public:

        /* 
        ===================================
             Constructors/Destructors
        ===================================
        */

        SpatialTransform() = default;

        virtual ~SpatialTransform() = default;

        SpatialTransform(const Quaternion& quaternion, const Vector3& translation) :  _quaternion(quaternion.getNormalized()), _translation(translation) { }

        SpatialTransform operator*(const SpatialTransform& other) const {
            // Compose spatial transforms directly via quaternion/translation, without
            // going through the 6x6 matrix representations.
            //
            // With this = (E2, r2), other = (E1, r1), the composed transform X3 = X2*X1
            // (apply other first, then this) satisfies, per block-multiplying eq. 2.24/2.25:
            //   E3 = E2 * E1
            //   r3 = r1 + E1^T * r2
            // i.e. compose the rotations, and add r1 to r2 rotated back into frame A
            Quaternion q3 = _quaternion * other._quaternion;
            Vector3 r3 = other._translation + (other._quaternion.getInverse() * _translation);
            return SpatialTransform(q3, r3);
        }

        SpatialMotion operator*(const SpatialMotion& v) const {
            // Apply the motion transform (eq. 2.24) directly, without building the 6x6 matrix:
            //   angular' = E * angular
            //   linear'  = E * (linear - r x angular)
            Vector3 angular = v.getAngularVelocity();
            Vector3 linear = v.getLinearVelocity();
            return SpatialMotion(
                _quaternion * angular,
                _quaternion * (linear - _translation.cross(angular))
            );
        }

        SpatialForce operator*(const SpatialForce& f) const {
            // Apply the force transform (eq. 2.25) directly, without building the 6x6 matrix:
            //   torque' = E * (torque - r x force)
            //   force'  = E * force
            Vector3 torque = f.getTorque();
            Vector3 force = f.getForce();
            return SpatialForce(
                _quaternion * (torque - _translation.cross(force)),
                _quaternion * force
            );
        }

        /**
        * @brief Builds the 6x6 spatial motion transform matrix X (eq. 2.24) for this transform, i.e. the
        * matrix satisfying X * v.getVector() == (*this) * v for a SpatialMotion v.
        */
        Matrix getMotionMatrix() const {
            Matrix E = _quaternion.getRotationMatrix();
            Matrix r_cross = Matrix33::skew(_translation);
            Matrix zero3(3, 3);

            Matrix top = E.horizontalConcatenate(zero3);
            Matrix bottom = (-(E * r_cross)).horizontalConcatenate(E);
            return top.verticalConcatenate(bottom);
        }

        /**
        * @brief Re-expresses a spatial-inertia-shaped 6x6 matrix in this transform's successor frame,
        * following the same direction convention as operator*: for B_X_A, `I_B = B_X_A.transformSpatialInertia(I_A)`
        * and `I_A = B_X_A.getInverse().transformSpatialInertia(I_B)`.
        *
        * Inertia maps a motion vector to a force vector, so it transforms by a congruence built from
        * *both* transforms (Featherstone, Rigid Body Dynamics Algorithms, 2008, eq. 2.66):
        *
        *   I_B = (B_X_A)* I_A (A_X_B) = X^-T I_A X^-1,   where X = getMotionMatrix() and X* = X^-T
        *
        * (f_B = X* f_A = X* I_A v_A = X* I_A X^-1 v_B). Note this is the *inverse* congruence to the
        * X^T I X that maps an inertia from the successor frame back into the predecessor frame.
        * @param I The 6x6 inertia, expressed in this transform's predecessor frame.
        * @return The same inertia, expressed in this transform's successor frame.
        */
        Matrix transformSpatialInertia(const Matrix& I) const {
            Matrix X_inv = getInverse().getMotionMatrix();
            return X_inv.getTranspose() * I * X_inv;
        }

        /**
        * @brief Rigid-body overload of transformSpatialInertia(const Matrix&), with the same direction
        * convention. Only valid for genuine rigid-body inertias: SpatialInertia stores the 10-parameter
        * (m, c, Ic) form, so an articulated-body inertia (21 independent parameters) must use the
        * Matrix overload above.
        */
        SpatialInertia transformSpatialInertia(const SpatialInertia& I) const {
            return SpatialInertia(transformSpatialInertia(I.getMatrix()));
        }

        /**
        * @brief The coordinate-transformation quaternion E. See the class comment for the exact
        * convention: `getRotation() * v` maps a vector's predecessor-frame components to its
        * successor-frame components.
        */
        Quaternion getRotation() const { return _quaternion; }

        /**
        * @brief The position of the successor frame's origin, expressed in predecessor coordinates.
        */
        Vector3 getTranslation() const { return _translation; }

        /**
        * @brief The inverse transform, mapping successor-frame quantities back into the
        * predecessor frame.
        */
        SpatialTransform getInverse() const {
            // Inverting (E, r): the predecessor's origin sits at -E r in successor coordinates,
            // and the coordinate map runs the other way, so E' = E^T.
            return SpatialTransform(_quaternion.getInverse(), -(_quaternion * _translation));
        }

    private:

        Quaternion _quaternion { };
        Vector3 _translation { };

};


} // namespace Stellarium
