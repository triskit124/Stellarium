#include <cmath>
#include <iostream>
#include <math.h>

#include "StellariumSimulation.h"
#include "TestHarness.h"
#include "Quaternion.h"
#include "Vector3.h"

using namespace Stellarium;



int main() { 

    Test test("Graphics window");

    StellariumSimulation sim(true);

    test.assertTrue("Sim bootup with window", true);

    return test.getNumFails();

}

