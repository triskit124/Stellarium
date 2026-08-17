#pragma once

// #include "Body.h"
#include "Body.h"
#include "Matrix.h"
#include "Vector3.h"
#include <stdexcept>
#include <vector>


namespace Stellarium
{

class Body;


/**
 * @brief A class representing an abtract joint.
 */
class Joint
{

    public:

        enum class Type {
            Free,
            Locked,
            Pin,
            Slider,
            Ball
        };

        struct Info {
            Type type;
            Body* parent;
            std::vector<Vector3> axes;
        };


        /*
        ===================================
             Constructors/Desctructors
        ===================================
        */


        /**
        * @brief Constructs a Joint object with the given name, mass, center of mass, and inertia.
        * @param name The name of the Joint.
        * @param mass The mass of the Joint.
        * @param cm The position of center of mass of the Joint.
        * @param inertia The inertia matrix of the Joint.
        */
        Joint(Body& parent, Body& child, std::vector<Vector3> joint_axes) : _parent(parent), _child(child), _joint_axes(joint_axes) {
        };

        /*
        ==============
            Methods
        ==============
        */

        virtual Matrix getMotionSubspace() = 0;

        // virtual Matrix getJointTransform() = 0;

        virtual constexpr int getDegreesOfFreedom() = 0;

        Body& getParent() { return _parent; }
        Body& getChild() { return _child; }



    private:

        Body& _parent;
        Body& _child;
        std::vector<Vector3> _joint_axes;


     

};



class PinJoint : public Joint
{

    public:

        PinJoint(Body& parent, Body& child, std::vector<Vector3> joint_axes) : Joint(parent, child, joint_axes) {
            if (joint_axes.size() != 1) {
                throw std::invalid_argument("");
            }
        }

        virtual constexpr int getDegreesOfFreedom() override { return 1; };

        virtual Matrix getMotionSubspace() override { 
            return Matrix { { _joint_axis[0], _joint_axis[1], _joint_axis[2], 0, 0, 0 } };
         };


    private:
         Vector3 _joint_axis;

};


} // end namespace Stellarium
