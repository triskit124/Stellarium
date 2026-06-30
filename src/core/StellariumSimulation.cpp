#include "StellariumSimulation.h"
#include "Body.h"
#include "Integrators.h"

#include <mutex>
#include <thread>
#include <iostream>
#include <memory>
#include <cassert>
#include <vector>

// #include "yaml-cpp/yaml.h"


namespace Stellarium
{

void StellariumSimulation::addGraphics()
{
#ifdef STELL_BUILD_RENDERING
    _graphics = std::make_unique<GraphicsEngine>();
#else
    throw std::invalid_argument("Graphics was enabled but stellarium has been built without rendering. Cannot continue.");
#endif
}

void StellariumSimulation::loadScenario(const std::string& filename)

{
    // YAML::Node config = YAML::LoadFile(filename);

    std::cout << "Loaded scenario file:  " << filename << std::endl;
}

Body* StellariumSimulation::addBody(const std::string& name, double mass, Vector3 cm, InertiaMatrix inertia, Vector3 pos, Vector3 vel, Quaternion att, Vector3 ang_vel)
{
    std::unique_ptr<Stellarium::Body> new_body = std::make_unique<Stellarium::Body>(name, mass, cm, inertia);

    new_body->setPosition(pos);
    new_body->setVelocity(vel);
    new_body->setAttitude(att);
    new_body->setAngularVelocity(ang_vel);

    _bodies.emplace_back(std::move(new_body));

    // std::cout << "Added body " << name << " to the simulation\n";

    return _bodies.back().get();
}

void StellariumSimulation::addIntegrator(const STELL_INTEGRATOR_TYPE& type, const double dt)
{
    if (type == STELL_INTEGRATOR_TYPE::rk4)
    {
        _integrator = std::make_unique<RK4>(dt);
    }
    else
    {
        throw std::invalid_argument("invalid integrator type");
    }

    // std::cout << "Added integrator of type " << type << " to the simulation\n";
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

    if (_graphics)
    {
        std::thread physics_thread([&]()
        {
            while (time() < t_f && _graphics->shouldRender())
            {
                {
                    std::lock_guard<std::mutex> lock(_graphics->poseMutex());
                    _step();
                }
            }
            
            _graphics->stopRendering();
        });

        _graphics->run();      // blocks on the main thread until the window closes
        physics_thread.join();
    }
    else 
    {
        while (time() < t_f)
        {
            _step();
        }
    }
}

#ifdef STELL_BUILD_RENDERING
Model* StellariumSimulation::loadModel(const std::string& path, Frame& frame)
{
    if (!_graphics)
    {
        throw std::runtime_error("Cannot load model: please call addGraphics() before loadModel().");
    }
    return _graphics->loadModel(path, frame);
}
#endif

StellariumSimulation::~StellariumSimulation()
{
    _bodies.clear();
}

} // end namespace Stellarium
