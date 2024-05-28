#include "StellariumSimulation.h"
#include "Body.h"
#include "Integrators.h"

#include <cstddef>
#include <iostream>
#include <array>
#include <memory>
#include <cassert>

// #include "yaml-cpp/yaml.h"


namespace Stellarium 
{

void StellariumSimulation::loadScenario(const std::string& filename)

{
    // YAML::Node config = YAML::LoadFile(filename);

    std::cout << "Loaded scenario file:  " << filename << std::endl;
}

Body* StellariumSimulation::addBody(const std::string& name, double mass, Vector3 cm, Matrix33 inertia, Vector3 pos, Vector3 vel, Quaternion att, Vector3 ang_vel)
{
    std::unique_ptr<Stellarium::Body> new_body = std::make_unique<Stellarium::Body>(name, mass, cm, inertia);

    new_body->setPosition(pos);
    new_body->setVelocity(vel);
    new_body->setAttitude(att);
    new_body->setAngularVelocity(ang_vel);

    std::array<double*, 13> states = new_body->getState();
    for (auto state : states)
    {
        _state.push_back(state);
    }
    _bodies.emplace_back(std::move(new_body));

    std::cout << "Added body " << name << " to the simulation\n";

    return _bodies.back().get();
}

void StellariumSimulation::addIntegrator(const std::string& type)
{
    if (type == "rk4")
    {
        _integrator = std::make_unique<rk4>();
    }
    else
    {
        throw std::invalid_argument("invalid integrator type");
    }

    std::cout << "Added integrator of type " << type << " to the simulation\n";
}

void StellariumSimulation::_step()
{
    if (!_integrator)
    {
        throw std::invalid_argument("Cannot step: no integrator has been set");
    }

    _state_dot.clear();

    // get the state derivatives for all bodies
    for (auto& body : _bodies)
    {
        std::array<double, STELL_BODY_STATE_SIZE> state_dot = body->getStateDot();
        for (double s : state_dot)
        {
            _state_dot.push_back(s);
        }
    }

    assert(_state_dot.size() == _bodies.size() * STELL_BODY_STATE_SIZE);

    // integrate the state
    _integrator->integrate(_state, _state_dot, _dt);

    // push the new state back to into the bodies
    for (size_t i = 0; i < _bodies.size(); ++i)
    {
        std::array<double, STELL_BODY_STATE_SIZE> state; 
        for (size_t j = 0; j < STELL_BODY_STATE_SIZE; j++)
        {
            state[j] = *_state[i*13 + j];
        }
        _bodies[i]->setState(state);

        std::cout << "Body " << _bodies[i]->name() << " position: " << _bodies[i]->getPosition()[0] << ", " << _bodies[i]->getPosition()[1] << ", " << _bodies[i]->getPosition()[2] << "\tVelocity:" << _bodies[i]->getVelocity()[0] << ", " << _bodies[i]->getVelocity()[1] << ", " << _bodies[i]->getVelocity()[2] << std::endl;
    }

    // advance the time
    _t += _dt;

}

void StellariumSimulation::run(double t)
{
    std::cout << "Running simulation for " << t << " seconds\n";
    while (time() < t)
    {
        std::cout << "Time: " << time() << " sec\n";
        _step();
    }
}

StellariumSimulation::~StellariumSimulation()
{
    _state.clear();
    _bodies.clear();
}

} // end namespace Stellarium