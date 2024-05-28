#ifndef STELL_SIM
#define STELL_SIM

#include "Constants.h"
#include "Body.h"
#include "Math.h"
#include "Integrators.h"

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
        StellariumSimulation() {};

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
        Body* addBody(const std::string& name, double mass, Vector3 cm, Matrix33 inertia, Vector3 pos, Vector3 vel, Quaternion att, Vector3 ang_vel);

        /**
         * @brief Adds an integrator to the simulation.
         * 
         * @param type The type of the integrator.
         */
        void addIntegrator(const std::string& type);

        /**
         * @brief Returns the size of the fixed integrator step.
         */
        double stepSize() const { return _dt; };

        /**
         * @brief Returns the current time of the simulation.
         */
        double time() const { return _t; };

        /**
         * @brief Sets the size of the fixed integrator step.
         * 
         * @param dt The size of the fixed integrator step
         */
        void stepSize(double dt) { _dt = dt; };

        /**
         * @brief Runs the simulation for a given time.
         * 
         * @param t The time to run the simulation for.
         */
        void run(double t);

        void _step();
   
    protected:
        std::vector<std::unique_ptr<Body>> _bodies {}; /**< vector of bodies in the simulation. */
        std::vector<double*> _state {}; /**< The vector of state variables for the bodies. */
        std::vector<double> _state_dot {}; /**< The vector of state derivatives for the bodies. */
        std::unique_ptr<Integrator> _integrator = nullptr; /**< The integrator used for advancing the simulation. */

        double _dt = 0.1; /**< The size of the fixed integrator step. */
        double _t = 0.0; /**< The current time of the simulation. */

    private:
};

} // end namespace Stellarium

#endif // end STELL_SIM