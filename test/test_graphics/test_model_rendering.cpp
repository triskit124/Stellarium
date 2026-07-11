#include "StellariumSimulation.h"
#include "TestHarness.h"
#include "Config.h"
#include "Vector3.h"
#include "InertiaMatrix.h"
#include "Quaternion.h"

#include <cmath>
#include <filesystem>

using namespace Stellarium;

int main() {

    Test test("Model rendering");

    // Create simulation with graphics enabled
    StellariumSimulation sim { };
    sim.addIntegrator(STELL_INTEGRATOR_TYPE::rk4, 0.001);
    sim.addGraphics();

    // Add a body with angular velocity so it spins
    Body* body = sim.addBody(
        "spinning_body",
        1.0,
        Vector3(),
        InertiaMatrix(),
        Vector3(0, 0, 0),
        Vector3(),
        Quaternion(),
        Vector3(0, 0, M_PI)
    );

    test.assertTrue("Body created", body != nullptr);

    // Load a model and associate it with the body
    std::string axes_model_path = (std::filesystem::path(STELL_PROJECT_ROOT) / "assets/axes.obj").string();
    std::string sphere_model_path = (std::filesystem::path(STELL_PROJECT_ROOT) / "assets/sphere.obj").string();
    Model* model = sim.loadModel(axes_model_path, *body);

    test.assertTrue("Model loaded", model);
    test.assertTrue("Model has frame", model->getFrame() == body);
    test.assertTrue("Model scale set", model->getScale() == Vector3(1.0, 1.0, 1.0));

    // Load a second model at a different position with no frame (static)
    Body* body2 = sim.addBody(
        "static_body_1",
        1.0,
        Vector3(),
        InertiaMatrix(),
        Vector3(5, 0, 0),
        Vector3(),
        Quaternion(),
        Vector3()
    );

    Model* model2 = sim.loadModel(sphere_model_path, *body2);

    test.assertTrue("Second model loaded", model2);
    test.assertTrue("Second model has frame", model2->getFrame() == body2);

    // Load a third model at a different position with no frame (static)
    Body* body3 = sim.addBody(
        "static_body_2",
        1.0,
        Vector3(),
        InertiaMatrix(),
        Vector3(0, 10, 0),
        Vector3(),
        Quaternion(),
        Vector3()
    );

    Model* model3 = sim.loadModel(sphere_model_path, *body3);

    test.assertTrue("third model loaded", body3);
    test.assertTrue("third model has frame", model3->getFrame() == body3);

    // Run the simulation for a while to visually inspect the rendering
    sim.run(3000);

    return test.getNumFails();
}
