#pragma once

#include "Matrix44.h"
#include "Quaternion.h"
#include "Vector3.h"

namespace Stellarium
{

/**
* @brief A class representing a rigid body transformation using quaternion and vector storage.
*/
class HomTransform
{
    public:

        /*
        ===================================
             Constructors/Destructors
        ===================================
        */

        /**
        * @brief Default constructor - creates identity transformation.
        */
        HomTransform() = default;

        /**
        * @brief Default destructor
        */
        virtual ~HomTransform() = default;

        /**
        * @brief Constructs from a quaternion and translation vector.
        * @param rotation The rotation quaternion.
        * @param translation The translation vector.
        */
        HomTransform(const Quaternion& rotation, const Vector3& translation)
            : _rotation(rotation.getNormalized()), _translation(translation) {}

        /**
        * @brief Constructs from a quaternion rotation.
        * @param rotation The rotation quaternion.
        */
        explicit HomTransform(const Quaternion& rotation)
            : _rotation(rotation.getNormalized()), _translation(Vector3(0.0, 0.0, 0.0)) {}

        /**
        * @brief Constructs from a translation vector.
        * @param translation The translation vector.
        */
        explicit HomTransform(const Vector3& translation)
            : _rotation(Quaternion(1.0, 0.0, 0.0, 0.0)), _translation(translation) {}

        /*
        ===================
              Methods
        ===================
        */

        /**
        * @brief Equality operator.
        * @param other The transformation to compare to.
        * @return True if the transformations are equal, false otherwise.
        */
        bool operator==(const HomTransform& other) const {
            return _rotation == other.getRotation() && _translation == other.getTranslation();
        }

        /**
        * @brief Gets the rotation quaternion.
        * @return The rotation quaternion.
        */
        Quaternion getRotation() const { return _rotation; }

        /**
        * @brief Gets the translation vector.
        * @return The translation vector.
        */
        Vector3 getTranslation() const { return _translation; }

        /**
        * @brief Sets the rotation quaternion.
        * @param rotation The new rotation quaternion.
        */
        void setRotation(const Quaternion& rotation) { _rotation = rotation.getNormalized(); }

        /**
        * @brief Sets the translation vector.
        * @param translation The new translation vector.
        */
        void setTranslation(const Vector3& translation) { _translation = translation; }

        /**
        * @brief Transforms a Vector3 by this transformation. Order of operations is rotation followed by translation.
        * @param v The vector to transform.
        * @return The transformed vector (rotation * v + translation).
        */
        Vector3 transform(const Vector3& v) const {
            return (_rotation * v) + _translation;
        }

        /**
        * @brief Returns the inverse of this transformation.
        * @return The inverse transformation.
        */
        HomTransform getInverse() const {
            Quaternion invRot = _rotation.getInverse();
            return HomTransform(invRot, -(invRot * _translation));
        }

        /**
        * @brief Composes this transformation with another.
        * @param other The transformation to compose with.
        * @return The composed transformation (this * other).
        */
        HomTransform operator*(const HomTransform& other) const {
            return HomTransform(
                _rotation * other.getRotation(),
                _rotation * other.getTranslation() + _translation
            );
        }

        /**
        * @brief Returns a 4x4 homogeneous transform matrix equivalent to this transform.
        * @return A 4x4 homogeneous transform matrix.
        */
        Matrix44 toMatrix() const {
            RotationMatrix rotMat = _rotation.getRotationMatrix();
            Matrix44 mat;
            mat[0] = Vector4(rotMat[0][0], rotMat[0][1], rotMat[0][2], _translation[0]);
            mat[1] = Vector4(rotMat[1][0], rotMat[1][1], rotMat[1][2], _translation[1]);
            mat[2] = Vector4(rotMat[2][0], rotMat[2][1], rotMat[2][2], _translation[2]);
            mat[3] = Vector4(0.0, 0.0, 0.0, 1.0);
            return mat;
        }

    private:

        Quaternion _rotation { 1.0, 0.0, 0.0, 0.0 }; // The quaternion portion of the transform
        Vector3 _translation { 0.0, 0.0, 0.0 }; // The translation portion of the transform

};

} // namespace Stellarium
