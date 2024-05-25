#include <iostream>

#include "StellariumSimulation.h"
#include "body.h"
#include "math.h"

using namespace Stellarium;

// int main()
// {



//     // create the simulation object
//     StellariumSimulation sim = StellariumSimulation();

//     sim.addIntegrator("rk4");

//     // add a vehicle
//     Body* vehicle = sim.addBody(
//         "vehicle1", 
//         1, Vector3(0, 0, 0), 
//         Matrix33(100, 0, 0, 0, 100, 0, 0, 0, 100),
//         Vector3(0, 0, 0),
//         Vector3(0, 0, 0),
//         Quaternion(1, 0, 0, 0),
//         Vector3(0, 0, 0)
//     );


//     vehicle->addForce(Vector3(1, 0, 1));

//     sim.run(10);
    

//     return 0;

// }
