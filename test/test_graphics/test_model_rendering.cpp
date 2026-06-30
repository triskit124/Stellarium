#include "StellariumSimulation.h"
#include "TestHarness.h"
#include "Config.h"
#include "Vector3.h"
#include "InertiaMatrix.h"
#include "Quaternion.h"

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
        Vector3(0, 0, -5),
        Vector3(),
        Quaternion(),
        Vector3(1, 1, 0)
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
        "static_body",
        1.0,
        Vector3(),
        InertiaMatrix(),
        Vector3(5, 0, -5),
        Vector3(),
        Quaternion(),
        Vector3()
    );

    Model* model2 = sim.loadModel(sphere_model_path, *body2);

    test.assertTrue("Second model loaded", model2);
    test.assertTrue("Second model has frame", model2->getFrame() == body2);

    // Run the simulation for a while to visually inspect the rendering
    sim.run(3000);

    return test.getNumFails();
}
