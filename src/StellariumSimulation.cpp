#include "StellariumSimulation.h"
#include "Body.h"
#include "Integrators.h"

#include <cstddef>
#include <iostream>
#include <array>
#include <memory>
#include <cassert>
#include <vector>

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

    _bodies.emplace_back(std::move(new_body));

    std::cout << "Added body " << name << " to the simulation\n";

    return _bodies.back().get();
}

void StellariumSimulation::addIntegrator(STELL_INTEGRATOR_TYPE type, const double dt)
{
    if (type == STELL_INTEGRATOR_TYPE::rk4)
    {
        _integrator = std::make_unique<RK4>(dt);
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

    std::vector<Body*> bodies;
    for (auto& body : _bodies)
    {
        bodies.push_back(body.get());
    }
    _integrator->integrate(bodies, _t);

}

void StellariumSimulation::run(double t)
{
    const double t_f = time() + t;
    std::cout << "Running simulation for " << t << " seconds\n";
    while (time() < t_f)
    {
        _step();

#if 0
        std::cout << "Time: " << time() << " sec\n";
        for (auto& body : _bodies)
        {
            std::cout << "Body: " << body->name() << " Position: ";
            body->getPosition().print();
            std::cout << "Body: " << body->name() << " Velocity: ";
            body->getVelocity().print();
        }
#endif
    }
}

StellariumSimulation::~StellariumSimulation()
{
    _bodies.clear();
}

} // end namespace Stellarium