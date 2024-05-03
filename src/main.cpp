#include <iostream>

#include "StellariumSimulation.h"
#include "body.h"
#include "math.h"

int main()
{

    // create the simulation object
    Stellarium::StellariumSimulation sim = Stellarium::StellariumSimulation();

    // add a vehicle with mass 1000 kg, center of mass at (0, 0, 0), and inertia matrix [100, 0, 0; 0, 100, 0; 0, 0, 100]
    sim.addBody("vehicle1", 1000, Stellarium::Vector3(0, 0, 0), Stellarium::Matrix33(100, 0, 0, 0, 100, 0, 0, 0, 100));

    // add a vehicle with mass 250 kg, center of mass at (1, 2, 3), and inertia matrix [100, 0, 0; 0, 100, 0; 0, 0, 100]
    sim.addBody("vehicle2", 250, Stellarium::Vector3(1, 2, 3), Stellarium::Matrix33(100, 0, 0, 0, 100, 0, 0, 0, 100));

    return 0;
}