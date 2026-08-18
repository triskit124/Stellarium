#include "StellariumSimulation.h"
#include "Body.h"
#include "InertiaMatrix.h"
#include "Integrators.h"
#include "Joint.h"
#include "Matrix.h"
#include "Matrix33.h"
#include "SpatialInertia.h"
#include "SpatialTransform.h"
#include "SpatialVector.h"
#include "SquareMatrix.h"
#include "Vector.h"
#include "Vector3.h"

#include <cstddef>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <memory>
#include <cassert>
#include <vector>
#include <chrono>


namespace Stellarium
{

StellariumSimulation::StellariumSimulation() {
    // Add a ficticious root body. This will serve as the inertial root for the simulation.
    addBody("root", SpatialInertia(0.0, Vector3(), InertiaMatrix()), {  });
}

void StellariumSimulation::addGraphics()
{
#ifdef STELL_BUILD_RENDERING
    _graphics = std::make_unique<GraphicsEngine>();
#else
    throw std::invalid_argument("Graphics was enabled but stellarium has been built without rendering. Cannot continue.");
#endif
}

Body* StellariumSimulation::addBody(const std::string& name, const SpatialInertia& spatial_inertia, const Joint::Info& joint_info)
{
    _bodies.emplace_back(std::make_unique<Stellarium::Body>(name, spatial_inertia, joint_info));

    // std::cout << "Added body " << name << " to the simulation\n";

    return _bodies.back().get();
}

void StellariumSimulation::addIntegrator(const STELL_INTEGRATOR_TYPE& type, const double dt)
{
    if (type == STELL_INTEGRATOR_TYPE::rk4)
    {
        _integrator = std::make_unique<RK4>(
            [this]() { return _getState(); }, 
            [this]() { return _getStateDot(); }, 
            [this](const Vector& v) { return _setState(v); }, 
            dt
        );
    }
    else
    {
        throw std::invalid_argument("invalid integrator type");
    }
}

void StellariumSimulation::_step()
{
    if (!_integrator)
    {
        throw std::invalid_argument("Cannot step: no integrator has been set");
    }

    _integrator->integrate(_t);

}

Vector StellariumSimulation::_getState() const
{
    Vector q;
    Vector q_dot;
    for (auto& body : _bodies)
    {
        if (Joint* joint = body->getJoint())
        {
            q.concatenate(joint->getQ());
            q_dot.concatenate(joint->getQDot());
        }
    }
    
    return q | q_dot;
}

Vector StellariumSimulation::_getStateDot() const
{
    Vector q_dot;
    for (auto& body : _bodies)
    {
        if (Joint* joint = body->getJoint())
        {
            q_dot.concatenate(joint->getQDot());
        }
    }
    Vector q_double_dot = _computeForwardDynamics();
    return q_dot | q_double_dot;
}

void StellariumSimulation::_setState(const Vector& s)
{
    size_t idx = 0;
    for (auto& body : _bodies)
    {
        if (Joint* joint = body->getJoint())
        {
            size_t dof = joint->getDegreesOfFreedom();
            Vector joint_state = Vector(dof);
            for (size_t i = 0; i < dof; ++i)
            {
                joint_state[i] = s[idx];
                idx++;
            }
            joint->setQ(joint_state);
        }
    }

    for (auto& body : _bodies)
    {
        if (Joint* joint = body->getJoint())
        {
            size_t dof = joint->getDegreesOfFreedom();
            Vector joint_state_dot = Vector(dof);
            for (size_t i = 0; i < dof; ++i)
            {
                joint_state_dot[i] = s[idx];
                idx++;
            }
            joint->setQDot(joint_state_dot);
        }
    }
}

Vector StellariumSimulation::_computeForwardDynamics() const
{
    // Forward dynamics via the Articulated Body Algorithm
    // See: Featherstone, Rigid Body Dynamics Algorithms, 2008, table 7.1
    //
    // _bodies[0] is the fixed base (no joint, zero velocity/acceleration). All other bodies are assumed
    // to appear after their parent in _bodies (i.e. parents are always processed before their children).

    std::map<Body*, SpatialVelocity> v { };
    std::map<Body*, SpatialTransform> i_X_p { };
    std::map<Body*, SpatialTransform> i_X_0 { };
    std::map<Body*, SpatialVelocity> c { };
    std::map<Body*, Matrix> I_A { };   // articulated-body inertia, size [6 x 6]
    std::map<Body*, Vector> p_A { };   // articulated-body bias force, size [6 x 1]
    std::map<Body*, Matrix> U { };     // size [6 x dof]
    std::map<Body*, Matrix> D_inv { }; // size [dof x dof]
    std::map<Body*, Vector> u { };     // size [dof x 1]
    std::map<Body*, Vector> a { };

    Body* base = _bodies[0].get();
    v[base] = SpatialVelocity();
    i_X_0[base] = SpatialTransform();

    // first pass: outward, root to tip
    for (size_t i = 1; i < _bodies.size(); ++i)
    {
        Body* body = _bodies[i].get();
        Joint* joint = body->getJoint();
        Body* parent = joint->getInfo().parent;

        SpatialTransform X_J = joint->getJointTransform();
        SpatialVelocity v_J = joint->getJointVelocity();

        i_X_p[body] = X_J * joint->getInfo().parent_to_joint;
        i_X_0[body] = i_X_p[body] * i_X_0[parent];

        v[body] = i_X_p[body] * v[parent] + v_J;
        c[body] = v[body].cross(v_J);

        I_A[body] = body->getSpatialInertia().getMatrix();

        SpatialForce f_ext = body->getExternalForce();
        p_A[body] = (v[body].cross(body->getSpatialInertia() * v[body]) - i_X_0[body] * f_ext).getVector();
    }

    // second pass: inward, tip to root -- fold each body's articulated inertia/bias force into its parent's
    for (size_t i = _bodies.size(); i-- > 1; )
    {
        Body* body = _bodies[i].get();
        Joint* joint = body->getJoint();
        Body* parent = joint->getInfo().parent;
        Matrix S = joint->getMotionSubspace();

        U[body] = I_A[body] * S;
        Matrix D = S.getTranspose() * U[body];
        u[body] = joint->getGeneralizedForce() - S.getTranspose() * p_A[body];
        D_inv[body] = SquareMatrix(D).getInverse();

        if (parent != base)
        {
            Matrix I_a = I_A[body] - U[body] * D_inv[body] * U[body].getTranspose();
            Vector p_a = p_A[body] + I_a * c[body].getVector() + U[body] * (D_inv[body] * u[body]);

            // Both propagate into the parent frame via the same congruence matrix X = i_X_p[body].getMotionMatrix():
            // I_parent += X^T I_a X (eq. 2.66-2.67), p_parent += X^T p_a (force-type quantities transform via X^-T = X*,
            // and going child->parent is the inverse direction, so it's X^T applied directly).
            I_A[parent] = I_A[parent] + i_X_p[body].transformInertiaToParent(I_a);
            Matrix X = i_X_p[body].getMotionMatrix();
            p_A[parent] = p_A[parent] + X.getTranspose() * p_a;
        }
    }

    // third pass: outward, root to tip
    a[base] = this->getConstantGravity();

    Vector qdd(0);
    for (size_t i = 1; i < _bodies.size(); ++i)
    {
        Body* body = _bodies[i].get();
        Joint* joint = body->getJoint();
        Body* parent = joint->getInfo().parent;
        Matrix S = joint->getMotionSubspace();

        Vector a_prime = (i_X_p[body] * SpatialVelocity(a[parent] + c[body].getVector())).getVector();

        Vector qdd_i = D_inv[body] * (u[body] - U[body].getTranspose() * a_prime);
        qdd = qdd | qdd_i;

        a[body] = a_prime + S * qdd_i;
    }

    return qdd;
}

void StellariumSimulation::run(double t)
{

    const double t_f = time() + t;

    double time_counter = 0.0;
    int step_counter = 0;

    if (_graphics)
    {
        std::thread physics_thread([&]()
        {
            // Cap the physics thread's real-time rate so it can't win every mutex
            // re-lock race against the render thread (non-fair std::mutex barging).
            const auto target_period = std::chrono::duration<double>(_integrator->getDeltaT());
            auto next_tick = std::chrono::steady_clock::now();

            while (time() < t_f && _graphics->shouldRender())
            {
                auto tic = std::chrono::steady_clock::now();

                {
                    std::lock_guard<std::mutex> lock(_graphics->poseMutex());
                    _step();
                }

                next_tick += std::chrono::duration_cast<std::chrono::steady_clock::duration>(target_period);
                std::this_thread::sleep_until(next_tick);

                auto toc = std::chrono::steady_clock::now();
                double seconds = std::chrono::duration<double>(toc - tic).count();
                time_counter += seconds;
                step_counter++;
                if (step_counter * target_period.count() >= 1.0) {
                    _graphics->setHudText("sim_time", std::format("time: {:.2f} s", time()), 10.0, 50.0, Vector3(1, 1, 1), 0.6);
                    _graphics->setHudText("physics_fps", std::format("physics: {:.2f} fps", step_counter / time_counter), 10.0, 70.0, Vector3(1, 1, 1), 0.6f);
                    time_counter = 0.0;
                    step_counter = 0;
                }
            }

            _graphics->stopRendering();
        });

        _graphics->run();      // blocks on the main thread until the window closes
        physics_thread.join();
    }
    else 
    {
        while (time() < t_f)
        {
            _step();
        }
    }
}

#ifdef STELL_BUILD_RENDERING
Model* StellariumSimulation::loadModel(const std::string& path, Frame& frame)
{
    if (!_graphics)
    {
        throw std::runtime_error("Cannot load model: please call addGraphics() before loadModel().");
    }
    return _graphics->loadModel(path, frame);
}
#endif

StellariumSimulation::~StellariumSimulation()
{
    _bodies.clear();
}

} // end namespace Stellarium
