#ifndef STELL_SIM
#define STELL_SIM

#include "body.h"
#include "math.h"
#include "integrators.h"

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
         * @brief Adds a celestial body to the simulation.
         * 
         * @param name The name of the body.
         * @param mass The mass of the body.
         * @param cm The center of mass of the body.
         * @param inertia The inertia matrix of the body.
         */
        void addBody(const std::string& name, double mass, Vector3 cm, Matrix33 inertia);

        /**
         * @brief Adds an integrator to the simulation.
         * 
         * @param type The type of the integrator.
         */
        void addIntegrator(const std::string& type);

        /**
         * @brief Advances the simulation by a given time step.
         * 
         * @param dt The time step to advance the simulation by.
         */
        void step(double dt);
   
    protected:
        std::vector<Body*> _bodies {}; /**< The vector of celestial bodies in the simulation. */
        std::vector<double*> _state {}; /**< The vector of state variables for the bodies. */
        std::vector<double> _state_dot {}; /**< The vector of state derivatives for the bodies. */
        Integrator* _integrator = nullptr; /**< The integrator used for advancing the simulation. */

    private:
};

} // end namespace Stellarium

#endif // end STELL_SIM