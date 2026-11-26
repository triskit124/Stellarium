/*
 * A single body on a free (6-DOF) joint, pushed by a constant inertial-frame force.
 *
 * This is the degenerate case of the articulated body algorithm -- one body, no parent coupling,
 * an identity motion subspace -- so it isolates the rigid-body equations of motion, the
 * external-force plumbing, and the free joint's configuration kinematics from any tree structure.
 */

#include "Config.h"
#include "InertiaMatrix.h"
#include "Joint.h"
#include "SpatialInertia.h"
#include "SpatialTransform.h"
#include "StellariumSimulation.h"
#include "TestHarness.h"
#include "Vector3.h"

using namespace Stellarium;

int main() {

    Test test("Simple propagation");


    StellariumSimulation sim;
    sim.addIntegrator(STELL_INTEGRATOR_TYPE::rk4, 0.1);
    
    // sim.addGraphics();
    // const std::string axes_model_path = (std::filesystem::path(STELL_PROJECT_ROOT) / "assets/axes.obj").string();
    // const std::string sphere_model_path = (std::filesystem::path(STELL_PROJECT_ROOT) / "assets/sphere.obj").string();

    Joint::Info free_joint;
    free_joint.type = Joint::Type::Free;
    free_joint.q_init = { 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
    free_joint.alpha_init = Vector(6);

    auto body = sim.addBody("body_1", SpatialInertia(1.0, Vector3(), InertiaMatrix()), free_joint);
    // sim.loadModel(sphere_model_path, body->getBodyFrame());
    // sim.loadModel(axes_model_path, body->getCenterOfMassFrame());

    test.assertEquals("Sim q size should be 7 (1 body, position + quaternion)", sim.getStateSize().first, 7);
    test.assertEquals("Sim alpha size should be 6 (1 body, angular+linear velocity)", sim.getStateSize().second, 6);

    body->addInertialFrameForce(Vector3(1,-1,1));

    sim.run(10.0);

    test.assertTrue("Body position 1", body->getPosition() == Vector3(50, -50, 50));
    test.assertTrue("Body velocity 1", body->getVelocity() == Vector3(10, -10, 10));
    test.assertTrue("Body angular velocity 1", body->getAngularVelocity() == Vector3());
    test.assertTrue("Body acceleration 1", body->getAcceleration() == Vector3(1,-1,1));
    test.assertTrue("Body angular acceleration 1", body->getAngularAcceleration() == Vector3());

    body->clearForces();
    body->addInertialFrameForce(Vector3(-1,1,-1));

    sim.run(10.0);

    test.assertTrue("Body position 2", body->getPosition() == Vector3(100, -100, 100));
    test.assertTrue("Body velocity 2", body->getVelocity() == Vector3(0, 0, 0));
    test.assertTrue("Body angular velocity 2", body->getAngularVelocity() == Vector3());
    test.assertTrue("Body acceleration 2", body->getAcceleration() == Vector3(-1,1,-1));
    test.assertTrue("Body angular acceleration 2", body->getAngularAcceleration() == Vector3());

    // at rest
    body->clearForces();
    sim.run(10.0);

    test.assertTrue("Body position 3", body->getPosition() == Vector3(100, -100, 100));
    test.assertTrue("Body velocity 3", body->getVelocity() == Vector3(0, 0, 0));
    test.assertTrue("Body angular velocity 3", body->getAngularVelocity() == Vector3());
    test.assertTrue("Body acceleration 3", body->getAcceleration() == Vector3());
    test.assertTrue("Body angular acceleration 3", body->getAngularAcceleration() == Vector3());

    test.assertTrue("Center of mass frame is still positioned properly", body->getBodyFrame().getTransformTo(body->getCenterOfMassFrame()) == SpatialTransform());

    return test.getNumFails();
}
