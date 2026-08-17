#pragma once

#include "Matrix.h"
#include "Matrix33.h"
#include "Quaternion.h"
#include "RotationMatrix.h"
#include "SpatialVector.h"
#include "Vector3.h"

namespace Stellarium
{

/**
* @brief A class representing a spatial transform.
* See: Featherstone, Rigid Body Dynamics Algorithms, 2008, pp. 22
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

        SpatialVelocity operator*(const SpatialVelocity& v) const {
            // Apply the motion transform (eq. 2.24) directly, without building the 6x6 matrix:
            //   angular' = E * angular
            //   linear'  = E * (linear - r x angular)
            Vector3 angular = v.getAngularVelocity();
            Vector3 linear = v.getLinearVelocity();
            return SpatialVelocity(
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
        * matrix satisfying X * v.getVector() == (*this) * v for a SpatialVelocity v.
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
        * @brief Transforms a spatial-inertia-shaped 6x6 matrix from the frame this transform maps TO (its
        * "child"/successor frame) back into the frame it maps FROM (its "parent"/predecessor frame).
        *
        * With X = this->getMotionMatrix() (the motion transform from parent to child, so that
        * v_child = X * v_parent), a spatial inertia transforms via the congruence I_parent = X^T * I_child * X
        * (Featherstone, Rigid Body Dynamics Algorithms, 2008, eq. 2.66-2.67: force-type quantities transform via
        * the dual/force transform X* = X^-T, and inertia maps velocity to momentum, so
        * momentum_parent = X^T * momentum_child = X^T * I_child * v_child = X^T * I_child * X * v_parent).
        * @param childInertia The spatial inertia (6x6), expressed in this transform's child/successor frame.
        * @return The same spatial inertia, expressed in this transform's parent/predecessor frame.
        */
        Matrix transformInertiaToParent(const Matrix& childInertia) const {
            Matrix X = getMotionMatrix();
            return X.getTranspose() * childInertia * X;
        }


    private:

        Quaternion _quaternion { };
        Vector3 _translation { };

};


} // namespace Stellarium
