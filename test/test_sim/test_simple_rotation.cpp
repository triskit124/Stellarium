#include "../../src/StellariumSimulation.h"
#include "../../src/TestHarness.h"
#include "../../src/AxisAngle.h"


using namespace Stellarium;

int main() {

    Test test("Simple propagation of rotational motion");


    StellariumSimulation sim;
    sim.addIntegrator(STELL_INTEGRATOR_TYPE::rk4, 0.1);

    auto body = sim.addBody("body_1");

    body->addBodyFrameTorque(Vector3(1,0,0));

    sim.run(10.0);

    test.assert("Body angular velocity 1", body->getAngularVelocity() == Vector3(10, 0, 0));

    std::cout << "FINISHED\n";
    // body->getAttitude().print();
    AxisAngle aa = AxisAngle(body->getAttitude());
    aa.getAxis().print();
    std::cout << aa.getAngle() << std::endl;

    // TODO: the rotational dynamics seem to be broken. Need to investigate

    return 0;
}