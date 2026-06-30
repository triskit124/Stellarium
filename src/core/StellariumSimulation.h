#pragma once

#include "Body.h"
#include "Vector3.h"
#include "InertiaMatrix.h"
#include "Quaternion.h"
#include "Integrators.h"

#ifdef STELL_BUILD_RENDERING
#include "GraphicsEngine.h"
#include "Model.h"
#endif

#include <memory>
#include <vector>


namespace Stellarium
{

/**
 * @brief The StellariumSimulation class
 */
class StellariumSimulation
{
    public:
        /**
         * @brief Constructs a StellariumSimulation object.
         */
        StellariumSimulation() = default;

        /**
         * @brief Destroys the StellariumSimulation object.
         */
        ~StellariumSimulation();

        /**
         * @brief Initializes the graphics engine.
         */
        void addGraphics();

        /**
         * @brief Loads a scenario from a file.
         *
         * @param filename The name of the file to load the scenario from.
         */
        void loadScenario(const std::string& filename);

        /**
         * @brief Adds a body to the simulation.
         *
         * @param name The name of the body.
         * @param mass The mass of the body.
         * @param cm The center of mass of the body.
         * @param inertia The inertia matrix of the body.
         * @param pos The position of the body.
         * @param vel The velocity of the body.
         * @param att The attitude of the body.
         * @param ang_vel The angular velocity of the body.
         */
        Body* addBody(const std::string& name,
                      double mass = 1.0,
                      Vector3 cm = Vector3(0,0,0),
                      InertiaMatrix inertia = InertiaMatrix(Vector3(1,0,0), Vector3(0,1,0), Vector3(0,0,1)),
                      Vector3 pos = Vector3(0,0,0),
                      Vector3 vel = Vector3(0,0,0),
                      Quaternion att = Quaternion(1,0,0,0),
                      Vector3 ang_vel = Vector3(0,0,0)
        );

        /**
         * @brief Adds an integrator to the simulation.
         *
         * @param type The type of the integrator.
         * @param dt The size of the fixed integrator step.
         */
        void addIntegrator(const STELL_INTEGRATOR_TYPE& type,  const double dt);

        /**
         * @brief Returns the current time of the simulation.
         */
        double time() const { return _t; };

        /**
         * @brief Runs the simulation for a given time.
         *
         * @param t The time to run the simulation for.
         */
        void run(double t);

#ifdef STELL_BUILD_RENDERING
        /**
         * @brief Loads a 3D model from a file and associates it with a frame so its transform is
         *        driven by the frame's pose each physics step.
         *
         * @param path  Path to the model file.
         * @param frame The frame whose pose drives this model's world transform.
         * @return Pointer to the stored Model.
         */
        Model* loadModel(const std::string& path, Frame& frame);

        GraphicsEngine* getGraphics() { return this->_graphics.get(); };
#endif

    protected:

    private:
        /**
        * @brief The bodies in the simulation.
        */
        std::vector<std::unique_ptr<Body>> _bodies {}; /**< vector of bodies in the simulation. */

        /**
        * @brief The integrator used for advancing the simulation.
        */
        std::unique_ptr<Integrator> _integrator = nullptr; /**< The integrator used for advancing the simulation. */

        /**
        * @brief The simulation time.
        */
        double _t = 0.0;

#ifdef STELL_BUILD_RENDERING
        std::unique_ptr<GraphicsEngine> _graphics { };
#endif

        /**
        * @brief Advances the simulation by one step. The size of the step is determined by the integrator.
        */
        void _step();

};

} // end namespace Stellarium
