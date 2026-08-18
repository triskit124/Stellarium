#pragma once

#include <functional>
#include <vector>

#include "Constants.h"
#include "Body.h"
#include "Vector.h"

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
        explicit Integrator(const std::function<Vector(void)>& state_getter, const std::function<Vector(void)>& state_dot_getter, const std::function<void(const Vector&)>& state_setter, const double dt = 1.0) : _state_getter(state_getter), _state_dot_getter(state_dot_getter), _state_setter(state_setter) {
            if (dt <= STELL_EPSILON)
            {
                throw std::invalid_argument("dt must be positive.");
            }
            _dt = dt;
        };

        /**
        * @brief Destructor.
        */
        virtual ~Integrator() = default;

        /**
        * @brief Integrates the state of the system.
        * @param bodies[in] The bodies to be integrated.
        * @param t[out] The simulation time variable. Gets updated by the integrator.
        */
        virtual void integrate(double& t) = 0;

        void setDeltaT(double dt);
        
        double getDeltaT() { return _dt; };

    protected:
        // /**
        // * @brief Collects the state vector for each body and stores the total simulation state in this->_state.
        // * @param bodies[in] The bodies to be integrated.
        // */
        // virtual void _computeStateVector(std::vector<Body*> bodies);

        // /**
        // * @brief Collects the state derivative vector for each body and stores the total simulation state derivative in this->_state_dot.
        // * @param bodies[in] The bodies to be integrated.
        // */
        // virtual void _computeStateDotVector(std::vector<Body*> bodies);

        /**
        * @brief The total state vector for all bodies in the simulation.
        */
        // std::vector<double*> _state {};

        /**
        * @brief The total state derivative vector for all bodies in the simulation.
        */
        // std::vector<double> _state_dot {};

        /**
        * @brief the integration step size
        */
        double _dt;

        const std::function<Vector(void)>& _state_getter;
        const std::function<Vector(void)>& _state_dot_getter;
        const std::function<void(const Vector&)>& _state_setter;

};

/**
 * @brief A class representing the RK4 integrator.
 */
class RK4 : public Integrator
{
    public:

        using Integrator::Integrator;

        /**
        * @brief Integrates the state of the system.
        * @param bodies[in] The bodies to be integrated.
        * @param t[out] The simulation time variable. Gets updated by the integrator.
        */
        virtual void integrate(double& t) override;

    protected:

    private:

};


} // end namespace Stellarium
