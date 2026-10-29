/*
 * Visual demo: a double pendulum swinging under gravity.
 *
 * This is not a meson test -- it opens a window and runs until it is closed (see test/meson.build).
 * The numeric verification of the same system lives in test/test_sim/test_pendulum.cpp.
 */

#include "InertiaMatrix.h"
#include "Joint.h"
#include "Quaternion.h"
#include "SpatialInertia.h"
#include "SpatialTransform.h"
#include "StellariumSimulation.h"
#include "Vector.h"
#include "Vector3.h"
#include "Config.h"

#include <cmath>
#include <filesystem>
#include <string>
#include <vector>

using namespace Stellarium;


int main() {

    StellariumSimulation sim;
    sim.addIntegrator(STELL_INTEGRATOR_TYPE::rk4, 0.001);
    sim.addConstantGravity({ 0, 0, -9.81 });
    sim.addGraphics();

    const std::string axes_model_path = (std::filesystem::path(STELL_PROJECT_ROOT) / "assets/axes.obj").string();

    constexpr double LINK_LENGTH = 5.0;
    constexpr double LINK_MASS = 10.0;

    // First link: pinned to the world at the origin, rotating about x.
    Joint::Info joint;
    joint.type = Joint::Type::Pin;
    joint.predecessor = nullptr; // attached to the world root
    joint.axes = { Vector3(1, 0, 0) };
    joint.predecessor_to_joint = SpatialTransform();
    joint.successor_to_joint = SpatialTransform(Quaternion(), Vector3(0, 0, LINK_LENGTH));
    joint.q_init = { 0.99 * M_PI};
    joint.alpha_init = { 0.0 };

    SpatialInertia inertia = SpatialInertia(LINK_MASS, Vector3(),InertiaMatrix(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1)));

    Body* body_1 = sim.addBody("body_1", inertia, joint);
    sim.loadModel(axes_model_path, body_1->getBodyFrame());

    // Second link: pinned to the far end of the first, a link length down its -z axis.
    joint.predecessor = body_1;
    joint.successor_to_joint = SpatialTransform(Quaternion(), Vector3(0.0, 0.0, LINK_LENGTH));
    joint.q_init = { 0.2};

    Body* body_2 = sim.addBody("body_2", inertia, joint);
    sim.loadModel(axes_model_path, body_2->getBodyFrame());

    // Third link
    // joint.predecessor = body_2;
    // joint.axes = { Vector3(0, 1, 0) };
    // joint.successor_to_joint = SpatialTransform(Quaternion(), Vector3(0.0, 0.0, LINK_LENGTH));
    // joint.q_init = { 0.2 };

    // Body* body_3 = sim.addBody("body_2", inertia, joint);
    // sim.loadModel(axes_model_path, body_3->getBodyFrame());

    // Runs until the window is closed.
    sim.run(3000);

    return 0;
}
