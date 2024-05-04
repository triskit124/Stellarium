#include "body.h"
#include "math.h"
#include "constants.h"

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
    Quaternion q_dot = _att * Quaternion(0.0, _ang_vel[0], _ang_vel[1], _ang_vel[2]) * 0.5;

    std::array<double, STELL_BODY_STATE_SIZE> state_dot {
        _vel[0],
        _vel[1],
        _vel[2],
        q_dot[0],
        q_dot[1],
        q_dot[2],
        q_dot[3],
        _force[0]/_mass,
        _force[1]/_mass,
        _force[2]/_mass,
        // euler equations, TODO: figure out non principal axis case:
        (_torque[0] - _ang_vel[1]*_ang_vel[2]*(_inertia[2][2] - _inertia[1][1]))/_inertia[0][0],
        (_torque[1] - _ang_vel[2]*_ang_vel[0]*(_inertia[0][0] - _inertia[2][2]))/_inertia[1][1],
        (_torque[2] - _ang_vel[0]*_ang_vel[1]*(_inertia[1][1] - _inertia[0][0]))/_inertia[2][2],
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