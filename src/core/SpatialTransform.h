#pragma once

#include "Matrix.h"
#include "Matrix33.h"
#include "RotationMatrix.h"
#include "Vector3.h"

namespace Stellarium
{

/**
* @brief A class representing a spatial motion-vector transform.
* See: Featherstone, Rigid Body Dynamics Algorithms, 2008, pp. 22
*/
class SpatialMotionTransform
{   
    public:

        /* 
        ===================================
             Constructors/Destructors
        ===================================
        */

        /**
        * @brief Constructs a SpatialMotionTransform object
        */
        SpatialMotionTransform(RotationMatrix rotation, Vector3 translation) :  _rotation(rotation), _translation(translation) { }

        Matrix getMatrix() const {
            // Matrix representation of this spatial transform
            // See: Featherstone, Rigid Body Dynamics Algorithms, 2008, eq. 2.24 
            return Matrix(
                (_rotation | Matrix::zeros(3)).verticalConcatenate((-_rotation * Matrix33::skew(_translation)) | _rotation) 
            );
        };


    private:

        RotationMatrix _rotation;
        Vector3 _translation;

};


/**
* @brief A class representing a spatial force-vector transform.
* See: Featherstone, Rigid Body Dynamics Algorithms, 2008, pp. 22
*/
class SpatialForceTransform
{   
    public:

        /* 
        ===================================
             Constructors/Destructors
        ===================================
        */

        /**
        * @brief Constructs a SpatialMotionTransform object
        */
        SpatialForceTransform(RotationMatrix rotation, Vector3 translation) : _rotation(rotation), _translation(translation) { }

        Matrix getMatrix() const {
            // Matrix representation of this spatial transform
            // See: Featherstone, Rigid Body Dynamics Algorithms, 2008, eq. 2.25 
            return Matrix(
                (_rotation | (-_rotation * Matrix33::skew(_translation))).verticalConcatenate(Matrix::zeros(3)| _rotation) 
            );
        };


    private:

        RotationMatrix _rotation;
        Vector3 _translation;

};


} // namespace Stellarium
