#pragma once

#include <vector>

#include "Constants.h"
#include "Body.h"

namespace Stellarium
{

/**
* @brief Supported integrator types.
*/
enum STELL_INTEGRATOR_TYPE
{
    rk4,
};

/**
 * @brief An abstract class representing an integrator.
 */
class Integrator
{
    public:

        /**
        * @brief Default constructor.
        * @param dt The integration step size.
        */
        Integrator(const double dt = 1.0) {
            if (dt <= STELL_EPSILON)
            {
                throw std::invalid_argument("dt must be positive.");
            }
            _dt = dt;
        };

        /**
        * @brief Destructor.
        */
        virtual ~Integrator();

        /**
        * @brief Integrates the state of the system.
        * @param bodies[in] The bodies to be integrated.
        * @param t[out] The simulation time variable. Gets updated by the integrator.
        */
        virtual void integrate(std::vector<Body*> bodies, double& t) = 0;

    protected:
        /**
        * @brief Collects the state vector for each body and stores the total simulation state in this->_state.
        * @param bodies[in] The bodies to be integrated.
        */
        virtual void _computeStateVector(std::vector<Body*> bodies);

        /**
        * @brief Collects the state derivative vector for each body and stores the total simulation state derivative in this->_state_dot.
        * @param bodies[in] The bodies to be integrated.
        */
        virtual void _computeStateDotVector(std::vector<Body*> bodies);

        /**
        * @brief The total state vector for all bodies in the simulation.
        */
        std::vector<double*> _state {};

        /**
        * @brief The total state derivative vector for all bodies in the simulation.
        */
        std::vector<double> _state_dot {};

        /**
        * @brief the integration step size
        */
        double _dt;

};

/**
 * @brief A class representing the RK4 integrator.
 */
class RK4 : public Integrator
{
    public:

        /**
        * @brief Default constructor.
        */
        RK4(const double dt = 1.0) : Integrator(dt) {};

        /**
        * @brief Integrates the state of the system.
        * @param bodies[in] The bodies to be integrated.
        * @param t[out] The simulation time variable. Gets updated by the integrator.
        */
        void integrate(std::vector<Body*> bodies, double& t) override;

    protected:

    private:

};


} // end namespace Stellarium
