#include "Joint.h"
#include "SpatialInertia.h"
#include "SpatialTransform.h"
#include "StellariumSimulation.h"
#include "TestHarness.h"
#include "Vector.h"
#include "Vector3.h"
#include "Config.h"

#include <cmath>
#include <vector>

using namespace Stellarium;

int main() {

    Test test("Double pendulum demo");

    // Create simulation with graphics enabled
    StellariumSimulation sim;
    sim.addIntegrator(STELL_INTEGRATOR_TYPE::rk4, 0.001);
    sim.addConstantGravity({ 0, 0, -9.81 });
    sim.addGraphics();

    Joint::Info joint;
    joint.parent = nullptr;
    joint.type = Joint::Type::Pin;
    joint.parent_to_joint = SpatialTransform();
    joint.axes = { Vector3(1, 0, 0) };
    joint.q_init = { 0.1 };
    joint.q_dot_init = { 0.0 };

    std::string axes_model_path = (std::filesystem::path(STELL_PROJECT_ROOT) / "assets/axes.obj").string();


    Body* body_1 = sim.addBody("body_1", SpatialInertia(), joint);

    sim.loadModel(axes_model_path, body_1->getBodyFrame());

    joint.parent = body_1;

    Body* body_2 = sim.addBody("body_2", SpatialInertia(), joint);

    sim.loadModel(axes_model_path, body_2->getBodyFrame());

    // Run the simulation for a while to visually inspect the rendering
    sim.run(3000);

    return test.getNumFails();
}
