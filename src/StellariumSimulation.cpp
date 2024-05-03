#include "StellariumSimulation.h"
#include "body.h"
#include "integrators.h"

#include <iostream>
#include <array>


namespace Stellarium 
{

void StellariumSimulation::addBody(const std::string& name, double mass, Vector3 cm, Matrix33 inertia)
{
    Stellarium::Body* new_body = new Body(name, mass, cm, inertia);
    _bodies.push_back(new_body);

    std::array<double*, 13> states = new_body->getState();
    for (auto state : states)
    {
        _state.push_back(state);
    }

    std::cout << "Added body " << name << " to the simulation\n";
}

void StellariumSimulation::addIntegrator(const std::string& type)
{
    if (type == "rk4")
    {
        _integrator = new rk4();

    }
    throw std::invalid_argument("invalid integrator type");
}

void StellariumSimulation::step(double dt)
{
    if (_integrator == nullptr)
    {
        throw std::invalid_argument("no integrator set");
    }

    for (auto body : _bodies)
    {
        _state_dot.clear();
        std::array<double, 13> state_dot = body->getStateDot();
        for (int i = 0; i < 13; i++)
        {
            _state_dot.push_back(state_dot[i]);
        }
    }

    _integrator->integrate(_state, _state_dot, dt);

}

StellariumSimulation::~StellariumSimulation()
{
    _state.clear();
    for (auto body : _bodies)
    {
        delete body;
    }
    _bodies.clear();
}

} // end namespace Stellarium