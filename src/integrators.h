#ifndef STELL_INTEGRATORS
#define STELL_INTEGRATORS

#include <vector>

#include "math.h"

namespace Stellarium 
{

class Integrator
{
    public:
        Integrator() {};
        virtual ~Integrator() {};
        virtual void integrate(std::vector<double*>& state, const std::vector<double>& state_dot, double dt) = 0;

};

class rk4 : public Integrator
{
    public:
        rk4() {};
        ~rk4() {};
        void integrate(std::vector<double*>& state, const std::vector<double>& state_dot, double dt);

    protected:

    private:

};


} // end namespace Stellarium

#endif // end STELL_INTEGRATORS