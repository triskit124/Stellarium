#pragma once

#include "Vector3.h"
#include "Matrix33.h"

namespace Stellarium
{

/**
 * @brief A class representing an inertia matrix.
 */
class InertiaMatrix : public Matrix33
{
    public:
        /**
         * @brief Default constructor for the InertiaMatrix object.
         */
        InertiaMatrix() : Matrix33() { } // no need to validate identity matrix

        /**
         * @brief Constructs an InertiaMatrix object with the given x, y, and z vectors.
         * @param x The 1st row of the matrix.
         * @param y The 2nd row of the matrix.
         * @param z The 3rd row of the matrix.
         */
        InertiaMatrix(Vector3 x, Vector3 y, Vector3 z) : Matrix33(x, y, z) {
            validate();
        }

        /**
         * @brief Converting constructor from a Matrix33.
         * @param m The Matrix33 object.
         */
        InertiaMatrix(const Matrix33& m) : Matrix33(m) {
            validate();
        }

    private:

        void validate() {
            if (!this->isSymmetric()) {
                throw std::invalid_argument("Inertia matrix must be symmetric");
            }
            if (!this->isPositiveDefinite()) {
                throw std::invalid_argument("Inertia matrix must be positive definite");
            }
        }

};

} // namespace Stellarium
