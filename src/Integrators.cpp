#include "Integrators.h"
#include "Body.h"
#include "Constants.h"
#include <cstddef>
#include <vector>

namespace Stellarium 
{


void RK4::_computeStateVector(std::vector<Body*> bodies)
{
   _state.clear();
   _state.reserve(bodies.size() * STELL_BODY_STATE_SIZE);

    // Collect the state derivatives of the system
    for (auto& body : bodies)
    {
        std::array<double*, STELL_BODY_STATE_SIZE> state_dot = body->getState();
        for (double* s : state_dot)
        {
            _state.push_back(s);
        }
    }
};

void RK4::_computeStateDotVector(std::vector<Body*> bodies)
{
   _state_dot.clear();
   _state_dot.reserve(bodies.size() * STELL_BODY_STATE_SIZE);

    // Collect the state derivatives of the system
    for (auto& body : bodies)
    {
        std::array<double, STELL_BODY_STATE_SIZE> state_dot = body->getStateDot();
        for (double s : state_dot)
        {
            _state_dot.push_back(s);
        }
    }
};


void RK4::integrate(std::vector<Body*> bodies, double& t)
{
    /* 
    ========================
        Initial  state
    ========================
    */

    _computeStateVector(bodies);
    _computeStateDotVector(bodies);

    std::vector<double> initial_state, k1, k2, k3, k4;
    initial_state.resize(_state.size());
    k1.resize(_state.size());
    k2.resize(_state.size());
    k3.resize(_state.size());
    k4.resize(_state.size());

    for (size_t i = 0; i < _state.size(); ++i)
    {
        initial_state[i] = *_state[i];
    }

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

    for (size_t i = 0; i < _state.size(); ++i)
    {
        *_state[i] = initial_state[i] + (_dt * k1[i] / 2);
    }

    _computeStateDotVector(bodies);

    k2 = _state_dot;

    /* 
    ============================
        Runge-Kutta step 3 
    ============================
    */
    for (size_t i = 0; i < _state.size(); ++i)
    {
        *_state[i] = initial_state[i] + (_dt * k2[i] / 2);
    }

    _computeStateDotVector(bodies);

    k3 = _state_dot;

    /* 
    ============================
        Runge-Kutta step 4 
    ============================
    */
    t += _dt/2;

    for (size_t i = 0; i < _state.size(); ++i)
    {
        *_state[i] = initial_state[i] + (_dt * k3[i]);
    }

    _computeStateDotVector(bodies);

    k4 = _state_dot;

    /* 
    ==============================
        Final integrated state
    ==============================
    */
    for (size_t i = 0; i < _state.size(); i++)
    {
        *_state[i] = initial_state[i] + (_dt / 6) * (k1[i] + 2*k2[i] + 2*k3[i] + k4[i]);
    }
};

Integrator::~Integrator()
{
    _state.clear();
    _state_dot.clear();
};

} // end namespace Stellarium