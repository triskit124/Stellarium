#include "StellariumSimulation.h"
#include "TestHarness.h"


using namespace Stellarium;

int main() {

    Test test("Simple propagation");


    StellariumSimulation sim;
    sim.addIntegrator(STELL_INTEGRATOR_TYPE::rk4, 0.1);

    auto body = sim.addBody("body_1");

    body->addInertialFrameForce(Vector3(1,-1,1));

    sim.run(10.0);

    test.assertTrue("Body position 1", body->getPosition() == Vector3(50, -50, 50));
    test.assertTrue("Body velocity 1", body->getVelocity() == Vector3(10, -10, 10));

    body->clearForces();
    body->addInertialFrameForce(Vector3(-1,1,-1));

    sim.run(10.0);

    test.assertTrue("Body position 2", body->getPosition() == Vector3(100, -100, 100));
    test.assertTrue("Body velocity 2", body->getVelocity() == Vector3(0, 0, 0));

    // at rest
    body->clearForces();
    sim.run(10.0);

    test.assertTrue("Body position 3", body->getPosition() == Vector3(100, -100, 100));
    test.assertTrue("Body velocity 3", body->getVelocity() == Vector3(0, 0, 0));

    return 0;
}