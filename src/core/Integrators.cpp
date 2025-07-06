#include "Integrators.h"
#include "Body.h"

#include <cstddef>
#include <vector>

namespace Stellarium 
{

void Integrator::_computeStateVector(std::vector<Body*> bodies)
{
   _state.clear();
   _state.reserve(bodies.size() * Body::STATE_SIZE);

    // Collect the state vector of the system
    for (auto& body : bodies)
    {
        std::array<double*, Body::STATE_SIZE> state = body->getState();
        for (double* s : state)
        {
            _state.push_back(s);
        }
    }
};

void Integrator::_computeStateDotVector(std::vector<Body*> bodies)
{
   _state_dot.clear();
   _state_dot.reserve(bodies.size() * Body::STATE_SIZE);

    // Collect the state derivatives of the system
    for (auto& body : bodies)
    {
        std::array<double, Body::STATE_SIZE> state_dot = body->getStateDot();
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

    // one last state dot call to normalize attitude quaternions and update derivatives
    _computeStateDotVector(bodies);
};

Integrator::~Integrator()
{
    _state.clear();
    _state_dot.clear();
};

} // end namespace Stellarium