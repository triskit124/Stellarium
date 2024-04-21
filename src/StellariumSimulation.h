#ifndef STELL_SIM
#define STELL_SIM

#include "body.h"
#include "math.h"
#include <vector>


namespace Stellarium 
{


class StellariumSimulation
{    
    public:
        StellariumSimulation();
        ~StellariumSimulation() {};

        void addBody(const std::string& name);
        void addBody(const std::string& name, double mass, Vector3 cm, Matrix33 inertia);
   
    protected:
        std::vector<Body> _bodies {};
        std::vector<double> _state {};
        std::vector<double> _state_dot {};

    private:
};

} // end namespace Stellarium

#endif // end STELL_SIM