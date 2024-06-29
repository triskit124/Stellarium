#include "../../src/StellariumSimulation.h"
#include "../../src/TestHarness.h"


using namespace Stellarium;

int main() {

    Test test("Simple propagation");


    StellariumSimulation sim;
    sim.addIntegrator(STELL_INTEGRATOR_TYPE::rk4, 0.1);

    auto body = sim.addBody("body_1");

    body->addForce(Vector3(1,-1,1));

    sim.run(10.0);

    test.assert("Body position 1", body->getPosition() == Vector3(50, -50, 50));
    test.assert("Body velocity 1", body->getVelocity() == Vector3(10, -10, 10));

    body->clearForces();
    body->addForce(Vector3(-1,1,-1));

    sim.run(10.0);

    test.assert("Body position 2", body->getPosition() == Vector3(100, -100, 100));
    test.assert("Body velocity 2", body->getVelocity() == Vector3(0, 0, 0));


    return 0;
}