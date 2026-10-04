/*
 * A single body on a free (6-DOF) joint, spun up by a constant body-frame torque.
 *
 * The counterpart to test_simple_translation: it exercises the rotational half of the free joint,
 * in particular the quaternion kinematics in FreeJoint::getQDot and the
 * re-normalization that follows every integration step.
 */

#include "AxisAngle.h"
#include "InertiaMatrix.h"
#include "Joint.h"
#include "SpatialInertia.h"
#include "StellariumSimulation.h"
#include "TestHarness.h"
#include "Vector3.h"

#include <cmath>
#include <iostream>


using namespace Stellarium;

int main() {

    Test test("Simple rotational motion");


    StellariumSimulation sim;
    sim.addIntegrator(STELL_INTEGRATOR_TYPE::rk4, 0.01);

    Joint::Info free_joint;
    free_joint.type = Joint::Type::Free;
    free_joint.q_init = FreeJoint::identityConfiguration();
    free_joint.alpha_init = Vector(6);

    // Unit inertia about every axis, so a torque about x produces no gyroscopic coupling and the
    // angular rate is simply the integral of the applied torque.
    auto body = sim.addBody("body_1", SpatialInertia(1.0, Vector3(), InertiaMatrix()), free_joint);

    body->addBodyFrameTorque(Vector3(1,0,0));

    sim.run(10.0);
    test.assertTrue("Body angular velocity 1", body->getAngularVelocity() == Vector3(10, 0, 0));

    // check rotation angle
    // Should be 50 radians about x-axis
    // This should wrap to 50 % 2pi = 6.0177 rad about x-axis OR 2pi - 6.0177 ~= 0.26548 rad about negative x-axis
    double angle = 2*M_PI - fmod(50.0, 2*M_PI);
    AxisAngle aa = AxisAngle(body->getAttitude());

    test.assertTrue("Body rotation 1 - axis", aa.getAxis() == Vector3(-1, 0, 0));
    test.assertTrue("Body rotation 1 - angle", std::abs(aa.getAngle() - angle) <= 1e-5);

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
    axis.setEpsilon(1e-8);

    test.assertTrue("Body rotation 2 - axis", axis == Vector3(-1, 0, 0));
    test.assertTrue("Body rotation 2 - angle", std::abs(aa.getAngle() - angle) <= 1e-5);

    return test.getNumFails();
}
