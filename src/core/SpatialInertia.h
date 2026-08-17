#pragma once

#include "InertiaMatrix.h"
#include "Matrix.h"
#include "SpatialVector.h"
#include "Vector3.h"
#include "Matrix33.h"
#include <stdexcept>
#include <string>

namespace Stellarium
{

/**
 * @brief A class representing an inertia matrix.
 */
class SpatialInertia
{
    public:

        SpatialInertia(double mass, Vector3 center_of_mass, InertiaMatrix cm_inertia)
        : _center_of_mass(center_of_mass), _cm_inertia(cm_inertia)
        {
            setMass(mass);
        }


        void setMass(double mass) {
            if (mass < 0.0) {
                throw std::invalid_argument("Mass must be non-negative.");
            }
            _mass = mass;
        }

        double getMass() const { return _mass; };

        /**
        * @brief Gets the center of mass of the body.
        * @return The center of mass of the body.
        */
        Vector3 getCenterOfMass() const { return _center_of_mass; };

        /**
        * @brief Sets the center of mass of the body.
        * @param cm The new center of mass of the body.
        */
        void setCenterOfMass(const Vector3& cm) { _center_of_mass = cm; };

        /**
        * @brief Gets the inertia matrix of the body.
        * @return The inertia matrix of the body.
        */
        InertiaMatrix getCenterOfMassInertia() const { return _cm_inertia; };

        /**
        * @brief Sets the inertia matrix of the body.
        * @param inertia The new inertia matrix of the body.
        */
        void setCenterOfMassInertia(const InertiaMatrix& cm_inertia) { _cm_inertia = cm_inertia; };

        Matrix getMatrix() const {
            // See Featherstone, Rigid Body Dynamics Algorithms, 2008, eq. 2.63
            Matrix33 c_cross = Matrix33::skew(_center_of_mass);
            Matrix33 c_cross_sq = c_cross * c_cross;

            Matrix33 top_left(
                Vector3(_cm_inertia[0] - _mass * c_cross_sq[0]),
                Vector3(_cm_inertia[1] - _mass * c_cross_sq[1]),
                Vector3(_cm_inertia[2] - _mass * c_cross_sq[2])
            );
            Matrix33 top_right = _mass * c_cross;
            Matrix33 bottom_left = -top_right; // == _mass * c_cross.getTranspose()

            return ((top_left | top_right).verticalConcatenate(bottom_left | (_mass * Matrix33())));
        }

        Matrix operator*(const Matrix& mat) {
            return  this->getMatrix() * mat;
        }

        SpatialForce operator*(const SpatialVelocity& v) const {
            // See Featherstone, Rigid Body Dynamics Algorithms, 2008, eq. 2.63.
            // p is the linear momentum, h is the angular momentum about the reference point (not the center of mass).
            Vector3 p = _mass * (v.getLinearVelocity() + v.getAngularVelocity().cross(_center_of_mass));
            Vector3 h = _cm_inertia * v.getAngularVelocity() + _center_of_mass.cross(p);
            return SpatialForce(h, p);
        }

    private:

        double _mass;
        Vector3 _center_of_mass;
        InertiaMatrix _cm_inertia;



};

} // namespace Stellarium
