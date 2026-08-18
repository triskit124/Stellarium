/*
 * A single body on a free (6-DOF) joint, pushed by a constant inertial-frame force.
 *
 * This is the degenerate case of the articulated body algorithm -- one body, no parent coupling,
 * an identity motion subspace -- so it isolates the rigid-body equations of motion, the
 * external-force plumbing, and the free joint's configuration kinematics from any tree structure.
 */

#include "InertiaMatrix.h"
#include "Joint.h"
#include "SpatialInertia.h"
#include "StellariumSimulation.h"
#include "TestHarness.h"
#include "Vector3.h"

using namespace Stellarium;

int main() {

    Test test("Simple propagation");


    StellariumSimulation sim;
    sim.addIntegrator(STELL_INTEGRATOR_TYPE::rk4, 0.1);

    Joint::Info free_joint;
    free_joint.type = Joint::Type::Free;
    free_joint.q_init = FreeJoint::identityConfiguration();
    free_joint.q_dot_init = Vector(FreeJoint::NUM_DOF);

    auto body = sim.addBody("body_1", SpatialInertia(1.0, Vector3(), InertiaMatrix()), free_joint);

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

    return test.getNumFails();
}
