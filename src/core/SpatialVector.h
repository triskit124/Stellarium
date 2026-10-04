#pragma once

#include "Vector.h"
#include "Vector3.h"
#include <stdexcept>

namespace Stellarium
{

// Abstract base class for SpatialVectors
class SpatialVector
{

    public:

        SpatialVector() = default;
        
        SpatialVector(const Vector3& angular_part, const Vector3& linear_part) : _angular_part(angular_part), _linear_part(linear_part) { };
        
        explicit SpatialVector(const Vector& vec) {
            if (vec.getSize() != 6) {
                throw std::invalid_argument("Wrong size to construct SpatialVector. Spatial Vectors must be size 6.");
            }
            _angular_part = { vec[0], vec[1], vec[2] };
            _linear_part = { vec[3], vec[4], vec[5] };
        }

        // Pure virtual descructor keeps this class abstract
        virtual ~SpatialVector() = 0;

        Vector3 getAngularPart() const { return _angular_part; }
        void setAngularPart(const Vector3& a) { _angular_part = a; }

        Vector3 getLinearPart() const { return _linear_part; }
        void setLinearPart(const Vector3& l) { _linear_part = l; }

        Vector getVector() const { return _angular_part.concatenate(_linear_part); };

    private:

        Vector3 _angular_part { };
        Vector3 _linear_part { };

};

// Pure abstract destructors need an out-of-class definition
inline SpatialVector::~SpatialVector() {}


class SpatialForce : public SpatialVector
{
    public:

        using SpatialVector::SpatialVector;

        SpatialForce operator+(const SpatialForce& f) const {
            return SpatialForce(this->getAngularPart() + f.getAngularPart(), this->getLinearPart() + f.getLinearPart());
        }

        SpatialForce operator-(const SpatialForce& f) const {
            return SpatialForce(this->getAngularPart() - f.getAngularPart(), this->getLinearPart() - f.getLinearPart());
        }
};


class SpatialMotion : public SpatialVector
{
    public:

        using SpatialVector::SpatialVector;

        SpatialMotion operator+(const SpatialMotion& f) const {
            return SpatialMotion(this->getAngularPart() + f.getAngularPart(), this->getLinearPart() + f.getLinearPart());
        }

        SpatialMotion operator-(const SpatialMotion& f) const {
            return SpatialMotion(this->getAngularPart() - f.getAngularPart(), this->getLinearPart() - f.getLinearPart());
        }

        SpatialMotion cross(const SpatialMotion& v) const {
            // See Featherstone, Rigid Body Dynamics Algorithms, 2008, eq. 2.33
            return SpatialMotion(
                this->getAngularPart().cross(v.getAngularPart()),
                this->getAngularPart().cross(v.getLinearPart()) + this->getLinearPart().cross(v.getAngularPart())
            );
        }

        SpatialForce cross(const SpatialForce& v) const {
            // See Featherstone, Rigid Body Dynamics Algorithms, 2008, eq. 2.34
            return SpatialForce(
                this->getAngularPart().cross(v.getAngularPart()) + this->getLinearPart().cross(v.getLinearPart()),
                this->getAngularPart().cross(v.getLinearPart())
            );
        }

        // Scalar product between Spatial motion and force vectors.
        // A scalar product is defined between M&F vectors, but not M&M or F&F
        // See Featherstone, Rigid Body Dynamics Algorithms, 2008, pp. 17
        double operator*(const SpatialForce& f) const {
            return this->getVector() * f.getVector();
        }

};


// Scalar product between Spatial motion and force vectors.
// A scalar product is defined between M&F vectors, but not M&M or F&F
// See Featherstone, Rigid Body Dynamics Algorithms, 2008, pp. 17
inline double operator*(const SpatialForce& f, const SpatialMotion& m) {
    return m * f;
}



} // namespace Stellarium
