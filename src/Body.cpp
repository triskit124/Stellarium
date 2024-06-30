#include "Body.h"
#include "Math.h"
#include "Constants.h"

#include <array>
#include <string>

namespace Stellarium 
{


std::array<double*, STELL_BODY_STATE_SIZE> Body::getState()
{
    std::array<double*, STELL_BODY_STATE_SIZE> state {
        _pos.x(),
        _pos.y(),
        _pos.z(),
        _att.w(),
        _att.x(),
        _att.y(),
        _att.z(),
        _vel.x(),
        _vel.y(),
        _vel.z(),
        _ang_vel.x(),
        _ang_vel.y(),
        _ang_vel.z(),
    };
    return state;
}

std::array<double, STELL_BODY_STATE_SIZE> Body::getStateDot() const
{
    /* 
    =============================================
        rigid body 6DOF equations of motion
    =============================================
    */

    // translational acceleration of body w.r.t. inertial frame, expressed in INERTIAL frame
    // _force is given in the body frame so we need to rotate it to the inertial frame first
    Vector3 accel = (_att.conjugate() * _force) / _mass;

    // angular acceleration of body w.r.t. inertial frame, expressed in BODY frame
    Vector3 ang_accel = _inertia.inverse() * (_torque - _ang_vel.cross(_inertia * _ang_vel));

    // quaternion derivative based on angular velocity
    // _att represents frame rotation from INERTIAL to BODY frame
    // _ang_vel is expressed in the BODY frame
    Quaternion q_dot = 0.5 * _att * Quaternion(0.0, _ang_vel[0], _ang_vel[1], _ang_vel[2]);

    std::array<double, STELL_BODY_STATE_SIZE> state_dot {
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

void Body::setState(const std::array<double, STELL_BODY_STATE_SIZE>& state)
{
    _pos = {state[0], state[1], state[2]};
    _att = {state[3], state[4], state[5], state[6]};
    _vel = {state[7], state[8], state[9]};
    _ang_vel = {state[10], state[11], state[12]};
}

} // end namespace Stellarium