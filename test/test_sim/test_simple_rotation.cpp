#include "StellariumSimulation.h"
#include "TestHarness.h"
#include "AxisAngle.h"
#include "Vector3.h"
#include <cmath>
#include <iostream>


using namespace Stellarium;

int main() {

    Test test("Simple rotational motion");


    StellariumSimulation sim;
    sim.addIntegrator(STELL_INTEGRATOR_TYPE::rk4, 0.01);

    auto body = sim.addBody("body_1");

    body->addBodyFrameTorque(Vector3(1,0,0));

    sim.run(10.0);
    test.assertTrue("Body angular velocity 1", body->getAngularVelocity() == Vector3(10, 0, 0));

    // check rotation angle
    // Should be 50 radians about x-axis
    // This should wrap to 50 % 2pi = 6.0177 rad about x-axis OR 2pi - 6.0177 ~= 0.26548 rad about negative x-axis
    double angle = 2*M_PI - fmod(50.0, 2*M_PI);
    AxisAngle aa = AxisAngle(body->getAttitude());
    
    test.assertTrue("Body rotation 1 - axis", aa.getAxis() == Vector3(-1, 0, 0));
    test.assertTrue("Body rotation 1 - angle", abs(aa.getAngle() - angle) <= 1e-5);

    body->clearForces();
    body->addBodyFrameTorque(Vector3(-1,0,0));

    sim.run(10.0);

    test.assertTrue("Body angular velocity 2", body->getAngularVelocity() == Vector3(0, 0, 0));

    // check rotation angle
    // Should be 100 radians about x-axis
    // This should wrap to 100 % 2pi = 5.7522 rad about x-axis OR 2pi - 5.7522 ~= 0.53096 rad about negative x-axis
    angle = 2*M_PI - fmod(100.0, 2*M_PI);
    aa = AxisAngle(body->getAttitude());
    Vector3 axis = aa.getAxis();
    axis.epsilon(1e-8);
    
    test.assertTrue("Body rotation 2 - axis", axis == Vector3(-1, 0, 0));
    test.assertTrue("Body rotation 2 - angle", abs(aa.getAngle() - angle) <= 1e-5);

    // at rest
    body->clearForces();
    sim.run(10.0);
    
    aa = AxisAngle(body->getAttitude());
    axis = aa.getAxis();
    axis.epsilon(1e-8);

    test.assertTrue("Body angular velocity 3", body->getAngularVelocity() == Vector3(0, 0, 0));
    test.assertTrue("Body rotation 3 - axis", axis == Vector3(-1, 0, 0));
    test.assertTrue("Body rotation 3 - angle", abs(aa.getAngle() - angle) <= 1e-5);

    return 0;
}