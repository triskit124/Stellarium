#ifndef STELL_SIM
#define STELL_SIM

#include "Constants.h"
#include "Body.h"
#include "Vector3.h"
#include "Matrix33.h"
#include "Quaternion.h"
#include "Integrators.h"

#ifdef STELL_BUILD_RENDERING
#include "GraphicsEngine.h"
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
        StellariumSimulation(bool graphics = false) {
            if (graphics) {
#ifdef STELL_BUILD_RENDERING
                _graphics = std::make_unique<GraphicsEngine>();
#else
                throw std::invalid_argument("graphics was enabled but stellarium has been built without rendering. Cannot continue.");
#endif
            }
        };

        /**
         * @brief Destroys the StellariumSimulation object.
         */
        ~StellariumSimulation();

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
                      Matrix33 inertia = Matrix33(Vector3(1,0,0), Vector3(0,1,0), Vector3(0,0,1)),
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
        void addIntegrator(STELL_INTEGRATOR_TYPE type,  const double dt);

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
        /**
        * @brief Pointer to the graphics engine.
        */
        std::unique_ptr<GraphicsEngine> _graphics = nullptr;
#endif

        /**
        * @brief Advances the simulation by one step. The size of the step is determined by the integrator.
        */
        void _step();

};

} // end namespace Stellarium

#endif // end STELL_SIM