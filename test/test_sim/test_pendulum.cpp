/*
 * Numeric verification of the Articulated Body Algorithm against an independent derivation.
 *
 * The reference here is NOT another spatial-algebra implementation -- it is a planar Lagrangian
 * written from scratch in plain trigonometry at the bottom of this file. The kinetic and potential
 * energies of the compound double pendulum are written out directly, and the equations of motion
 * are recovered from them by finite-differencing the Euler-Lagrange equation
 *
 *     d/dt (dT/dqdot_i) - dT/dq_i + dV/dq_i = 0
 *  => sum_j M_ij qddot_j  +  sum_j (d2T/dqdot_i dq_j) qdot_j  -  dT/dq_i  +  dV/dq_i = 0
 *
 * so that nothing in the reference shares code, conventions, or sign choices with src/core.
 * A transposed rotation, a flipped gravity sign, or a mis-transformed bias force in the ABA all
 * show up as a mismatch.
 *
 * Reference for the implementation under test: Featherstone, Rigid Body Dynamics Algorithms, 2008,
 * table 7.1.
 */

#include "Body.h"
#include "InertiaMatrix.h"
#include "Integrators.h"
#include "Joint.h"
#include "Quaternion.h"
#include "SpatialInertia.h"
#include "SpatialTransform.h"
#include "Matrix.h"
#include "SquareMatrix.h"
#include "StellariumSimulation.h"
#include "TestHarness.h"
#include "Vector.h"
#include "Vector3.h"

#include <array>
#include <cmath>
#include <sstream>
#include <string>

using namespace Stellarium;

namespace {

/*
=====================================================================================
    Link parameters, shared by the simulation and by the independent Lagrangian.

    Each link hangs down its own -z axis: the joint is at the link's frame origin, the
    centre of mass sits at -l_c along z, and the next joint at -L along z. All rotation
    is about x, so the motion is planar in y-z and only the xx inertia matters.
=====================================================================================
*/

constexpr double GRAVITY = 9.81;

struct Link {
    double mass;
    double length;      // distance from this link's joint to the next one
    double com_offset;  // distance from this link's joint to its centre of mass
    double inertia_xx;  // about the centre of mass
};

constexpr Link LINK_1 { 1.3, 0.80, 0.35, 0.11 };
constexpr Link LINK_2 { 0.7, 0.60, 0.25, 0.04 };

SpatialInertia spatialInertiaOf(const Link& link)
{
    // Diagonal so that the off-plane inertia components cannot couple into the planar motion,
    // which is what lets the 1-DOF-per-link Lagrangian below be an exact reference.
    return SpatialInertia(link.mass,
                          Vector3(0.0, 0.0, -link.com_offset),
                          InertiaMatrix(Vector3(link.inertia_xx, 0, 0),
                                        Vector3(0, link.inertia_xx + 0.05, 0),
                                        Vector3(0, 0, 0.02)));
}

/**
 * @brief Builds an n-link planar chain of pin joints about x, each hung off the previous one.
 */
std::vector<Body*> buildChain(StellariumSimulation& sim, const std::vector<Link>& links)
{
    std::vector<Body*> bodies;
    Body* parent = nullptr; // nullptr == attached to the world
    double parent_length = 0.0;

    for (size_t i = 0; i < links.size(); ++i)
    {
        Joint::Info info;
        info.type = Joint::Type::Pin;
        info.parent = parent;
        info.axes = { Vector3(1, 0, 0) };
        info.parent_to_joint = SpatialTransform(Quaternion(), Vector3(0.0, 0.0, -parent_length));
        info.q_init = { 0.0 };
        info.q_dot_init = { 0.0 };

        Body* body = sim.addBody("link_" + std::to_string(i + 1), spatialInertiaOf(links[i]), info);
        bodies.push_back(body);
        parent = body;
        parent_length = links[i].length;
    }
    return bodies;
}

void setJointState(const std::vector<Body*>& bodies, const std::vector<double>& q, const std::vector<double>& q_dot)
{
    for (size_t i = 0; i < bodies.size(); ++i)
    {
        bodies[i]->getJoint()->setQ(Vector { q[i] });
        bodies[i]->getJoint()->setQDot(Vector { q_dot[i] });
    }
}

/*
=====================================================================================
    Independent reference: planar energies, written in bare trigonometry.
=====================================================================================
*/

/**
 * @brief Kinetic energy of the chain at the given joint angles/rates.
 *
 * Joint angles are relative, so link i's absolute angle is the running sum of q[0..i].
 * With link i's joint at (0, p_y, p_z) and absolute angle a_i, its centre of mass sits at
 * (0, p_y + l_c sin a_i, p_z - l_c cos a_i); differentiating gives the velocity below.
 */
double kineticEnergy(const std::vector<Link>& links, const std::vector<double>& q, const std::vector<double>& q_dot)
{
    double energy = 0.0;

    double angle = 0.0;      // absolute angle of the current link
    double rate = 0.0;       // absolute angular rate of the current link
    double joint_vy = 0.0;   // velocity of the current link's joint
    double joint_vz = 0.0;

    for (size_t i = 0; i < links.size(); ++i)
    {
        angle += q[i];
        rate += q_dot[i];

        const double com_vy = joint_vy + links[i].com_offset * std::cos(angle) * rate;
        const double com_vz = joint_vz + links[i].com_offset * std::sin(angle) * rate;

        energy += 0.5 * links[i].mass * (com_vy * com_vy + com_vz * com_vz);
        energy += 0.5 * links[i].inertia_xx * rate * rate;

        // Advance to the next joint, a distance `length` down this link's -z axis.
        joint_vy += links[i].length * std::cos(angle) * rate;
        joint_vz += links[i].length * std::sin(angle) * rate;
    }

    return energy;
}

/**
 * @brief Gravitational potential energy of the chain, with the first joint as the datum.
 */
double potentialEnergy(const std::vector<Link>& links, const std::vector<double>& q)
{
    double energy = 0.0;

    double angle = 0.0;
    double joint_z = 0.0;

    for (size_t i = 0; i < links.size(); ++i)
    {
        angle += q[i];
        energy += links[i].mass * GRAVITY * (joint_z - links[i].com_offset * std::cos(angle));
        joint_z -= links[i].length * std::cos(angle);
    }

    return energy;
}

/**
 * @brief The joint-space mass matrix, M(q), where T = 0.5 * qdot^T M qdot.
 *
 * T is exactly quadratic in qdot, so M can be read off by evaluating T at unit rates rather than
 * by differencing -- no step size, no truncation error, no cancellation.
 */
SquareMatrix massMatrix(const std::vector<Link>& links, const std::vector<double>& q)
{
    const size_t n = links.size();

    auto unit = [n](size_t i) { std::vector<double> v(n, 0.0); v[i] = 1.0; return v; };
    auto unit_pair = [n](size_t i, size_t j) { std::vector<double> v(n, 0.0); v[i] += 1.0; v[j] += 1.0; return v; };

    SquareMatrix M(n);
    for (size_t i = 0; i < n; ++i) {
        M[i][i] = 2.0 * kineticEnergy(links, q, unit(i));
    }
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            const double off = kineticEnergy(links, q, unit_pair(i, j))
                             - kineticEnergy(links, q, unit(i))
                             - kineticEnergy(links, q, unit(j));
            M[i][j] = off;
            M[j][i] = off;
        }
    }
    return M;
}

/**
 * @brief Solves the Euler-Lagrange equations for qddot.
 *
 *   d/dt (dT/dqdot_i) - dT/dq_i + dV/dq_i = 0
 *   with  dT/dqdot_i = sum_j M_ij qdot_j,
 *
 * gives  sum_j M_ij qddot_j = dT/dq_i - dV/dq_i - sum_{j,k} (dM_ij/dq_k) qdot_k qdot_j.
 *
 * Only first derivatives are differenced (of M, T and V, all of which are computed exactly), so
 * central differences at h = 1e-5 are good to roughly 1e-10.
 */
std::vector<double> lagrangianAcceleration(const std::vector<Link>& links,
                                           const std::vector<double>& q,
                                           const std::vector<double>& q_dot)
{
    const size_t n = links.size();
    const double h = 1e-5;

    auto perturb = [](std::vector<double> x, size_t i, double d) { x[i] += d; return x; };

    // dM/dq_k, one matrix per k.
    std::vector<Matrix> dM_dq;
    for (size_t k = 0; k < n; ++k) {
        dM_dq.push_back((massMatrix(links, perturb(q, k, h)) - massMatrix(links, perturb(q, k, -h))) / (2.0 * h));
    }

    Vector rhs(n);
    for (size_t i = 0; i < n; ++i)
    {
        const double dT_dq = (kineticEnergy(links, perturb(q, i, h), q_dot) - kineticEnergy(links, perturb(q, i, -h), q_dot)) / (2.0 * h);
        const double dV_dq = (potentialEnergy(links, perturb(q, i, h)) - potentialEnergy(links, perturb(q, i, -h))) / (2.0 * h);

        double velocity_product = 0.0;
        for (size_t j = 0; j < n; ++j) {
            for (size_t k = 0; k < n; ++k) {
                velocity_product += dM_dq[k][i][j] * q_dot[k] * q_dot[j];
            }
        }

        rhs[i] = dT_dq - dV_dq - velocity_product;
    }

    const Vector qdd = massMatrix(links, q).getInverse() * rhs;

    std::vector<double> result(n);
    for (size_t i = 0; i < n; ++i) {
        result[i] = qdd[i];
    }
    return result;
}

} // end anonymous namespace


int main() {

    Test test("Pendulum dynamics");

    /*
    ==========================================================================
        1. Single pendulum against its closed-form equation of motion.
    ==========================================================================
        A compound pendulum swinging about a fixed pivot obeys
            I_pivot * theta_ddot = -m g l_c sin(theta),
        with I_pivot = I_cm + m l_c^2 by the parallel axis theorem.
    */
    {
        const std::vector<Link> links { LINK_1 };

        StellariumSimulation sim;
        sim.addIntegrator(STELL_INTEGRATOR_TYPE::rk4, 1e-3);
        sim.addConstantGravity({ 0, 0, -GRAVITY });
        std::vector<Body*> bodies = buildChain(sim, links);

        const double inertia_about_pivot = LINK_1.inertia_xx + LINK_1.mass * LINK_1.com_offset * LINK_1.com_offset;

        bool all_match = true;
        double worst = 0.0;
        for (double theta : { 0.0, 0.1, 0.7, -1.4, 2.9, 3.14 })
        {
            for (double theta_dot : { 0.0, 2.5, -4.0 })
            {
                setJointState(bodies, { theta }, { theta_dot });
                const double expected = -(LINK_1.mass * GRAVITY * LINK_1.com_offset / inertia_about_pivot) * std::sin(theta);
                const double actual = sim.getGeneralizedAcceleration()[0];
                worst = std::max(worst, std::abs(actual - expected));
                all_match = all_match && std::abs(actual - expected) <= 1e-12;
            }
        }
        test.assertTrue("single pendulum matches closed form (worst error " + std::to_string(worst) + ")", all_match);

        // The rate must not enter a single-pendulum's acceleration: if it does, the velocity-product
        // (Coriolis/centrifugal) term is wrong.
        setJointState(bodies, { 0.9 }, { 0.0 });
        const double at_rest = sim.getGeneralizedAcceleration()[0];
        setJointState(bodies, { 0.9 }, { 6.0 });
        const double spinning = sim.getGeneralizedAcceleration()[0];
        test.assertEquals("single pendulum acceleration is independent of rate", at_rest, spinning, 1e-12);

        // Small-angle period.
        const double expected_period = 2.0 * M_PI * std::sqrt(inertia_about_pivot / (LINK_1.mass * GRAVITY * LINK_1.com_offset));
        setJointState(bodies, { 1e-3 }, { 0.0 });
        double previous = 1e-3;
        double zero_crossing_time = 0.0;
        while (sim.time() < 4.0)
        {
            const double before = sim.time();
            sim.run(1e-3);
            const double angle = bodies[0]->getJoint()->getQ()[0];
            if (previous > 0.0 && angle <= 0.0 && zero_crossing_time == 0.0)
            {
                // Linear interpolation onto the crossing.
                zero_crossing_time = before + 1e-3 * (previous / (previous - angle));
            }
            previous = angle;
        }
        test.assertEquals("small-angle period", 4.0 * zero_crossing_time, expected_period, 1e-3);
    }

    /*
    ==========================================================================
        2. Double pendulum against the finite-differenced Lagrangian.
    ==========================================================================
    */
    {
        const std::vector<Link> links { LINK_1, LINK_2 };

        StellariumSimulation sim;
        sim.addIntegrator(STELL_INTEGRATOR_TYPE::rk4, 1e-3);
        sim.addConstantGravity({ 0, 0, -GRAVITY });
        std::vector<Body*> bodies = buildChain(sim, links);

        // A spread of states, including ones with large rates so the velocity-product terms and
        // the articulated-inertia coupling both carry real weight.
        const std::vector<std::pair<std::vector<double>, std::vector<double>>> states {
            { { 0.0,  0.0 }, { 0.0,  0.0 } },
            { { 0.1,  0.0 }, { 0.0,  0.0 } },
            { { 0.0,  0.4 }, { 0.0,  0.0 } },
            { { 0.6, -0.9 }, { 0.0,  0.0 } },
            { { 0.6, -0.9 }, { 1.5, -2.0 } },
            { { -1.2, 2.1 }, { -3.0, 4.5 } },
            { { 2.8,  0.3 }, { 0.7,  0.2 } },
            { { 1.5707963267948966, 1.5707963267948966 }, { 2.0, -1.0 } },
        };

        bool all_match = true;
        double worst = 0.0;
        for (const auto& [q, q_dot] : states)
        {
            setJointState(bodies, q, q_dot);
            const Vector actual = sim.getGeneralizedAcceleration();
            const std::vector<double> expected = lagrangianAcceleration(links, q, q_dot);

            for (size_t i = 0; i < expected.size(); ++i)
            {
                worst = std::max(worst, std::abs(actual[i] - expected[i]));
            }
            for (size_t i = 0; i < expected.size(); ++i)
            {
                all_match = all_match && std::abs(actual[i] - expected[i]) <= 1e-8;  // the finite-difference floor of the reference, not of the ABA
            }
        }
        std::ostringstream worst_str;
        worst_str << std::scientific << worst;
        test.assertTrue("double pendulum matches Lagrangian (worst error " + worst_str.str() + ")", all_match);
    }

    /*
    ==========================================================================
        3. Energy conservation over a simulated swing.
    ==========================================================================
        Chaotic motion from a raised start, integrated with RK4 -- any secular energy drift here
        means the dynamics are not derivable from the energies above.
    */
    {
        const std::vector<Link> links { LINK_1, LINK_2 };

        StellariumSimulation sim;
        sim.addIntegrator(STELL_INTEGRATOR_TYPE::rk4, 1e-3);
        sim.addConstantGravity({ 0, 0, -GRAVITY });
        std::vector<Body*> bodies = buildChain(sim, links);

        const std::vector<double> q0 { 2.2, -1.1 };
        const std::vector<double> v0 { 0.0, 0.0 };
        setJointState(bodies, q0, v0);

        const double initial_energy = kineticEnergy(links, q0, v0) + potentialEnergy(links, q0);

        double worst_drift = 0.0;
        for (int i = 0; i < 100; ++i)
        {
            sim.run(0.05);

            const std::vector<double> q { bodies[0]->getJoint()->getQ()[0], bodies[1]->getJoint()->getQ()[0] };
            const std::vector<double> v { bodies[0]->getJoint()->getQDot()[0], bodies[1]->getJoint()->getQDot()[0] };
            const double energy = kineticEnergy(links, q, v) + potentialEnergy(links, q);
            worst_drift = std::max(worst_drift, std::abs(energy - initial_energy));
        }

        test.assertTrue("energy is conserved over 5 s of chaotic motion (worst drift " + std::to_string(worst_drift) + " J)",
                        worst_drift <= 1e-6);
    }

    /*
    ==========================================================================
        4. Forward kinematics: joint coordinates -> world poses.
    ==========================================================================
        The render layer reads Frames, not joint coordinates, so the pose written by
        updateFrames() gets its own check against the same planar trigonometry used above.
    */
    {
        const std::vector<Link> links { LINK_1, LINK_2 };

        StellariumSimulation sim;
        sim.addIntegrator(STELL_INTEGRATOR_TYPE::rk4, 1e-3);
        sim.addConstantGravity({ 0, 0, -GRAVITY });
        std::vector<Body*> bodies = buildChain(sim, links);

        const double t1 = 0.6;
        const double t2 = -0.9;
        const double w1 = 1.5;
        const double w2 = -2.0;
        setJointState(bodies, { t1, t2 }, { w1, w2 });
        sim.updateFrames();

        // Link 1 pivots at the origin; link 2 hangs off the far end of link 1.
        const Vector3 expected_pos_2(0.0, LINK_1.length * std::sin(t1), -LINK_1.length * std::cos(t1));

        test.assertTrue("link 1 sits at the world origin", bodies[0]->getPosition() == Vector3(0, 0, 0));
        test.assertTrue("link 2 sits at the end of link 1", bodies[1]->getPosition() == expected_pos_2);

        // Attitudes are absolute: joint angles compose down the chain.
        test.assertTrue("link 1 attitude is the first joint angle",
                        bodies[0]->getAttitude() == Quaternion(Vector3(1, 0, 0), t1));
        test.assertTrue("link 2 attitude is the sum of the joint angles",
                        bodies[1]->getAttitude() == Quaternion(Vector3(1, 0, 0), t1 + t2));

        // Angular velocity is reported in the body frame, and about x it is just the rate sum.
        test.assertTrue("link 1 angular velocity", bodies[0]->getAngularVelocity() == Vector3(w1, 0, 0));
        test.assertTrue("link 2 angular velocity", bodies[1]->getAngularVelocity() == Vector3(w1 + w2, 0, 0));

        // Linear velocity is reported in the inertial frame: differentiate expected_pos_2.
        test.assertTrue("link 1 is pinned and so does not translate", bodies[0]->getVelocity() == Vector3(0, 0, 0));
        test.assertTrue("link 2 velocity is the tip velocity of link 1",
                        bodies[1]->getVelocity() == Vector3(0.0,
                                                            LINK_1.length * std::cos(t1) * w1,
                                                            LINK_1.length * std::sin(t1) * w1));

        // The centre of mass frame rides half a com_offset down the link's own -z axis.
        const Vector3 expected_com_2 = expected_pos_2 + Vector3(0.0,
                                                                LINK_2.com_offset * std::sin(t1 + t2),
                                                                -LINK_2.com_offset * std::cos(t1 + t2));
        test.assertTrue("link 2 centre of mass frame", bodies[1]->getCenterOfMassFrame().getPosition() == expected_com_2);
    }

    return test.getNumFails();
}
