#include "Body.h"

#include <iostream>

namespace Stellarium 
{


std::array<double*, Body::STATE_SIZE> Body::getState()
{
    _att.normalize();

    std::array<double*, Body::STATE_SIZE> state {
        &_pos[0],
        &_pos[1],
        &_pos[2],
        &_att.w,
        &_att.x,
        &_att.y,
        &_att.z,
        &_vel[0],
        &_vel[1],
        &_vel[2],
        &_ang_vel[0],
        &_ang_vel[1],
        &_ang_vel[2],
    };
    return state;
}

std::array<double, Body::STATE_SIZE> Body::getStateDot()
{
    /* 
    =============================================
        rigid body 6DOF equations of motion
    =============================================
    */

    _att.normalize();

    // translational acceleration of body w.r.t. inertial frame, expressed in INERTIAL frame
    // _force is given in the body frame so we need to rotate it to the inertial frame first
    Vector3 accel = (_att.getConjugate() * _force) / _mass;

    // angular acceleration of body w.r.t. inertial frame, expressed in BODY frame
    Vector3 ang_accel = _inertia.inverse() * (_torque - _ang_vel.cross(_inertia * _ang_vel));

    // quaternion derivative based on angular velocity
    // _att represents frame rotation from INERTIAL to BODY frame
    // _ang_vel is expressed in the BODY frame
    Quaternion q_dot = 0.5 * _att * Quaternion(0.0, _ang_vel[0], _ang_vel[1], _ang_vel[2], false);

    std::array<double, Body::STATE_SIZE> state_dot {
        _vel[0],
        _vel[1],
        _vel[2],
        q_dot[0],
        q_dot[1],
        q_dot[2],
        q_dot[3],
        accel[0],
        accel[1],
        accel[2],
        ang_accel[0],
        ang_accel[1],
        ang_accel[2],
    };
    return state_dot;
}


} // end namespace Stellarium