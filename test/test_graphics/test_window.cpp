#include <cmath>
#include <iostream>
#include <math.h>

#include "StellariumSimulation.h"
#include "TestHarness.h"
#include "Quaternion.h"
#include "Vector3.h"
#include "Config.h"

using namespace Stellarium;



int main() { 

    Test test("Graphics window");

    StellariumSimulation sim;
    sim.addGraphics();

    Body* body = sim.addBody(
            "dummy_body",
            1.0,
            Vector3(),
            InertiaMatrix(),
            Vector3(),
            Vector3(),
            Quaternion(),
            Vector3()
        );

    // Load a model and associate it with the body
    std::string model_path = (std::filesystem::path(STELL_PROJECT_ROOT) / "assets/backpack/backpack.obj").string();
    Model* model = sim.loadModel(model_path, *body);

    test.assertTrue("Sim bootup with window", true);
    sim.getGraphics()->run();

    return test.getNumFails();

}

