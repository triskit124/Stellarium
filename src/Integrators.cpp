#include "Integrators.h"

namespace Stellarium 
{

void rk4::integrate(std::vector<double*>& state, const std::vector<double>& state_dot, double dt)
{
    // Compute the RK4 integration step
    std::vector<double> k1, k2, k3, k4;
    k1.resize(state.size());
    k2.resize(state.size());
    k3.resize(state.size());
    k4.resize(state.size());

    for (size_t i = 0; i < state.size(); i++)
    {
        k1[i] = state_dot[i];
    }

    for (size_t i = 0; i < state.size(); i++)
    {
        k2[i] = state_dot[i] + dt*k1[i]/2.0;
    }

    for (size_t i = 0; i < state.size(); i++)
    {
        k3[i] = state_dot[i] + dt*k2[i]/2.0;
    }

    for (size_t i = 0; i < state.size(); i++)
    {
        k4[i] = state_dot[i] + dt*k3[i];
    }

    for (size_t i = 0; i < state.size(); i++)
    {
        *state[i] = *state[i] + (dt / 6.0) * (k1[i] + 2*k2[i] + 2*k3[i] + k4[i]);
    }
};

} // end namespace Stellarium