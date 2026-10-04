#include "Integrators.h"
#include "Vector.h"

#include <cstddef>
#include <stdexcept>
#include <string>

namespace Stellarium 
{

void Integrator::setDeltaT(double dt)
{
    if (dt <= 0)
    {
        throw std::invalid_argument("Cannot set dt of " + std::to_string(dt) + ". Must be positive.");
    }
    _dt = dt;
}

void RK4::integrate(double& t)
{
    /* 
    ========================
        Initial  state
    ========================
    */

    Vector _state = _state_getter();
    Vector _state_dot = _state_dot_getter();

    Vector initial_state = _state;
    Vector k1 = Vector(_state.getSize());
    Vector k2 = Vector(_state.getSize());
    Vector k3 = Vector(_state.getSize());
    Vector k4 = Vector(_state.getSize());

    /* 
    ============================
        Runge-Kutta step 1 
    ============================
    */
    k1 = _state_dot;

    /* 
    ============================
        Runge-Kutta step 2 
    ============================
    */
    t += _dt/2;

    for (size_t i = 0; i < _state.getSize(); ++i)
    {
        _state[i] = initial_state[i] + (_dt * k1[i] / 2);
    }

    _state_setter(_state);
    _state_dot = _state_dot_getter();

    k2 = _state_dot;

    /* 
    ============================
        Runge-Kutta step 3 
    ============================
    */
    for (size_t i = 0; i < _state.getSize(); ++i)
    {
        _state[i] = initial_state[i] + (_dt * k2[i] / 2);
    }

    _state_setter(_state);
    _state_dot = _state_dot_getter();

    k3 = _state_dot;

    /* 
    ============================
        Runge-Kutta step 4 
    ============================
    */
    t += _dt/2;

    for (size_t i = 0; i < _state.getSize(); ++i)
    {
        _state[i] = initial_state[i] + (_dt * k3[i]);
    }

    _state_setter(_state);
    _state_dot = _state_dot_getter();

    k4 = _state_dot;

    /* 
    ==============================
        Final integrated state
    ==============================
    */
    for (size_t i = 0; i < _state.getSize(); i++)
    {
        _state[i] = initial_state[i] + (_dt / 6) * (k1[i] + 2*k2[i] + 2*k3[i] + k4[i]);
    }

    _state_setter(_state);

    // one last state dot call to normalize attitude quaternions and update derivatives
    _state_dot = _state_dot_getter();

};


} // end namespace Stellarium