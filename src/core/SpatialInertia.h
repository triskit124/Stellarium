#pragma once

#include "InertiaMatrix.h"
#include "Matrix.h"
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
            return ((_cm_inertia | Matrix::zeros(3)).verticalConcatenate(Matrix::zeros(3) | _mass * Matrix33()));
        }

        Matrix operator*(const Matrix& mat) {
            if (mat.getNumRows() != 6) {
                throw std::invalid_argument("Cannot multiply SpatialInertia matrix with another matrix that has" + std::to_string(mat.getNumRows()) + " rows. Must have 6.");
            }
            return  this->getMatrix() * mat;
        }

    private:

        double _mass;
        Vector3 _center_of_mass;
        InertiaMatrix _cm_inertia;



};

} // namespace Stellarium
