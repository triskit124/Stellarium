#pragma once

#include "Constants.h"
#include "InertiaMatrix.h"
#include "Matrix.h"
#include "SpatialVector.h"
#include "Vector3.h"
#include "Matrix33.h"
#include <cstdlib>
#include <stdexcept>

namespace Stellarium
{

/**
 * @brief A class representing an inertia matrix.
 */
class SpatialInertia
{
    public:

        SpatialInertia() = default;

        SpatialInertia(double mass, const Vector3& center_of_mass, const InertiaMatrix& cm_inertia)
        : _center_of_mass(center_of_mass), _cm_inertia(cm_inertia)
        {
            setMass(mass); // validates mass before setting it
        }

        // construct a SpatialInertia matrix from a raw 6x6 Matrix. 
        // requires extracting relevant mass, center of mass, cm inertia from the matrix
        // See Featherstone, Rigid Body Dynamics Algorithms, 2008, eq. 2.63 for the definition of the SpatialInertia structure
        explicit SpatialInertia(const Matrix& m) {
            if (m.getNumRows() != 6 || m.getNumCols() != 6) {
                throw std::invalid_argument("Cannot construct Spatial inertia from given matrix. Wrong size.");
            }

            // extract mass from bottom right element of the given matrix, which by definition is the scalar mass
            _mass = m[5][5];

            if (_mass < 0.0) {
                throw std::runtime_error("Cannot construct Spatial inertia from given matrix. Mass is negative.");
            }

            // extract center of mass vector from the upper-right 3x3 block of the given matrix, which by definition is m*c_cross
            if (std::abs(_mass) <= STELL_EPSILON)
            {
                _center_of_mass = { 0.0, 0.0, 0.0 };
            }
            else 
            {
                // m*c_cross is skew-symmetric, so read each component from the
                // antisymmetric average of the two off-diagonal entries that carry it.
                double c_x = 0.5 * (m[2][4] - m[1][5]) / _mass;
                double c_y = 0.5 * (m[0][5] - m[2][3]) / _mass;
                double c_z = 0.5 * (m[1][3] - m[0][4]) / _mass;
                _center_of_mass = { c_x, c_y, c_z };
            }

            // extract centroidal inertia from the top-left 3x3 block of the given matrix, which is by definition Ic + m * c_cross * c_cross^T
            Matrix33 upper_left_block = {
                { m[0][0], m[0][1], m[0][2] },
                { m[1][0], m[1][1], m[1][2] },
                { m[2][0], m[2][1], m[2][2] },
            };
            _cm_inertia = InertiaMatrix(upper_left_block - _mass * Matrix33::skew(_center_of_mass) * Matrix33::skew(_center_of_mass).getTranspose());
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

            return ((top_left.horizontalConcatenate(top_right)).verticalConcatenate(bottom_left.horizontalConcatenate(_mass * Matrix33())));
        }

        SpatialForce operator*(const SpatialMotion& v) const {
            // See Featherstone, Rigid Body Dynamics Algorithms, 2008, eq. 2.63.
            // p is the linear momentum, h is the angular momentum about the reference point (not the center of mass).
            Vector3 p = _mass * (v.getLinearPart() + v.getAngularPart().cross(_center_of_mass));
            Vector3 h = _cm_inertia * v.getAngularPart() + _center_of_mass.cross(p);
            return SpatialForce(h, p);
        }

    private:

        double _mass { };
        Vector3 _center_of_mass { };
        InertiaMatrix _cm_inertia { };

};

} // namespace Stellarium
