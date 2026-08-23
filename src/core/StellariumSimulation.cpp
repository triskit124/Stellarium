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

namespace {

/**
* @brief Appends every element of `chunk` onto `out`.
*
* The flat simulation state is assembled from variable-length per-joint blocks. Vector is fixed
* size once it is non-empty, so the assembly happens in a std::vector and is converted once at the
* end via toVector().
*/
void append(std::vector<double>& out, const Vector& chunk)
{
    for (size_t i = 0; i < chunk.getSize(); ++i)
    {
        out.push_back(chunk[i]);
    }
}

Vector toVector(const std::vector<double>& values)
{
    Vector v(values.size());
    for (size_t i = 0; i < values.size(); ++i)
    {
        v[i] = values[i];
    }
    return v;
}

} // end anonymous namespace


StellariumSimulation::StellariumSimulation() {
    // Add a ficticious root body. This will serve as the inertial root for the simulation. It has
    // no joint (Joint::Info::parent defaults to nullptr, which Body::attachToParent reads as
    // "fixed base"), so it never appears in the state vector or in the dynamics passes.
    //
    // Constructed directly rather than through addBody() because addBody() re-parents null parents
    // onto this very body, which does not exist yet.
    _bodies.emplace_back(std::make_unique<Stellarium::Body>("root", SpatialInertia(0.0, Vector3(), InertiaMatrix()), Joint::Info { }));
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
    Joint::Info info = joint_info;

    // A null parent means "attach to the world", not "second fixed base". Without this, the body
    // would be created jointless and the dynamics passes -- which assume every body past the root
    // has a joint -- would dereference a null Joint*.
    if (info.parent == nullptr)
    {
        info.parent = getBase();
    }

    _bodies.emplace_back(std::make_unique<Stellarium::Body>(name, spatial_inertia, info));

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

    updateFrames();
}

/*
================================================================================================
    State vector layout

    [ q for every jointed body ; alpha for every jointed body ], each block in _bodies order.

    Following Featherstone, Rigid Body Dynamics Algorithms, 2008, eq. 3.8, the second half is the
    *velocity coordinate* vector alpha, not d(q)/dt -- the two differ whenever a joint uses a
    redundant parameterization. The halves are therefore sized independently by nq and nv, which
    differ for FreeJoint (nq = 7, nv = 6).
================================================================================================
*/

Vector StellariumSimulation::_getState() const
{
    std::vector<double> q { };
    std::vector<double> alpha { };
    for (auto& body : _bodies)
    {
        if (Joint* joint = body->getJoint())
        {
            append(q, joint->getQ());
            append(alpha, joint->getAlpha());
        }
    }

    append(q, toVector(alpha));
    return toVector(q);
}

Vector StellariumSimulation::_getStateDot() const
{
    std::vector<double> state_dot { };
    for (auto& body : _bodies)
    {
        if (Joint* joint = body->getJoint())
        {
            append(state_dot, joint->getQDot());
        }
    }

    append(state_dot, _computeForwardDynamics());
    return toVector(state_dot);
}

void StellariumSimulation::_setState(const Vector& s)
{
    size_t num_q = 0;
    size_t num_alpha = 0;
    for (auto& body : _bodies)
    {
        if (Joint* joint = body->getJoint())
        {
            num_q += joint->getQ().getSize();
            num_alpha += joint->getDegreesOfFreedom();
        }
    }

    if (s.getSize() != num_q + num_alpha)
    {
        throw std::invalid_argument("Cannot set state: expected a vector of size " + std::to_string(num_q + num_alpha) + " but got " + std::to_string(s.getSize()));
    }

    size_t q_idx = 0;
    size_t alpha_idx = num_q;

    for (auto& body : _bodies)
    {
        Joint* joint = body->getJoint();
        if (!joint)
        {
            continue;
        }

        Vector q = Vector(joint->getQ().getSize());
        for (size_t i = 0; i < q.getSize(); ++i)
        {
            q[i] = s[q_idx];
            q_idx++;
        }
        joint->setQ(q);

        Vector alpha = Vector(joint->getDegreesOfFreedom());
        for (size_t i = 0; i < alpha.getSize(); ++i)
        {
            alpha[i] = s[alpha_idx];
            alpha_idx++;
        }
        joint->setAlpha(alpha);

        // Pull q back onto the joint's configuration manifold -- integrating a unit quaternion
        // component-wise walks it off the unit sphere.
        joint->normalizeConfiguration();
    }
}

Vector StellariumSimulation::_computeForwardDynamics() const
{
    // Forward dynamics via the Articulated Body Algorithm
    // See: Featherstone, Rigid Body Dynamics Algorithms, 2008, table 7.1
    //
    // _bodies[0] is the fixed base (no joint, zero velocity). All other bodies are assumed to
    // appear after their parent in _bodies (i.e. parents are always processed before their
    // children)
    
    // TODO: add topological sorting to ensure that _bodies is properly sorted

    std::map<const Body*, SpatialVelocity> v { }; // spatial velociy of each body, size [6 x 1]
    std::map<const Body*, SpatialTransform> i_X_p { }; // transform from body i's parent to body i
    std::map<const Body*, SpatialTransform> i_X_0 { }; // transform from root body to body i
    std::map<const Body*, SpatialVelocity> c { }; // velocity-product accelerations for each body, size [6 x 1]
    std::map<const Body*, Matrix> I_A { };   // articulated-body inertia, size [6 x 6] Note: ABIs have 21 independent parameters, not 10, so we shouldn't represent them with the SpatialInertia class
    std::map<const Body*, SpatialForce> p_A { };   // articulated-body bias force, size [6 x 1]
    std::map<const Body*, Matrix> U { };     // subexpression given in Featherstone eq. 7.43, size [6 x dof]
    std::map<const Body*, Matrix> D_inv { }; // subexpression given in Featherstone eq. 7.44, size [dof x dof]
    std::map<const Body*, Vector> u { };     // subexpression given in Featherstone eq. 7.45, size [dof x 1]
    std::map<const Body*, SpatialVelocity> a { }; // spatial acceleraion of each body, size [6 x 1]

    const Body* base = getBase();
    v[base] = SpatialVelocity();
    i_X_0[base] = SpatialTransform();

    // first pass: outward, root to tip
    // calculates velocity-product accelerations (c) and bias forces (p)
    for (size_t i = 1; i < _bodies.size(); ++i)
    {
        Body* body = _bodies[i].get();
        Joint* joint = body->getJoint();
        if (!joint)
        {
            throw std::runtime_error("Body '" + body->getName() + "' has no joint. Only the root body may be jointless.");
        }
        const Body* parent = joint->getInfo().parent;

        SpatialVelocity v_J = joint->getJointVelocity();

        i_X_p[body] = joint->getInfo().child_to_joint.getInverse() * joint->getJointTransform() * joint->getInfo().parent_to_joint;
        i_X_0[body] = i_X_p.at(body) * i_X_0.at(parent);

        v[body] = i_X_p.at(body) * v.at(parent) + v_J;
        c[body] = v.at(body).cross(v_J);

        I_A[body] = body->getSpatialInertia().getMatrix();

        // eq. 7.16. f_ext has to be expressed in body coordinates about the body frame origin:
        // the body-frame accumulator already is, and the inertial-frame one only needs rotating
        // (not a full Plucker transform) because both accumulators act at that same origin.
        const Quaternion E = i_X_0.at(body).getRotation();
        const SpatialForce f_ext_inertial = body->getInertialFrameExternalForce();
        const SpatialForce f_ext = body->getBodyFrameExternalForce()
                                 + SpatialForce(E * f_ext_inertial.getTorque(), E * f_ext_inertial.getForce());

        p_A[body] = v.at(body).cross(body->getSpatialInertia() * v.at(body)) - f_ext;
    }

    // second pass: inward, tip to root -- fold each body's articulated inertia/bias force into its parent's
    for (size_t i = _bodies.size(); i-- > 1; )
    {
        Body* body = _bodies[i].get();
        Joint* joint = body->getJoint();
        const Body* parent = joint->getInfo().parent;
        Matrix S = joint->getMotionSubspace();

        U[body] = I_A.at(body) * S;
        Matrix D = S.getTranspose() * U.at(body);
        u[body] = joint->getGeneralizedForce() - S.getTranspose() * p_A.at(body).getVector();
        D_inv[body] = SquareMatrix(D).getInverse();

        if (parent != base)
        {
            Matrix I_a = I_A.at(body) - U.at(body) * D_inv.at(body) * U.at(body).getTranspose();
            SpatialForce p_a = p_A.at(body) + SpatialForce(I_a * c.at(body).getVector() + U.at(body) * (D_inv.at(body) * u.at(body)));

            // i_X_p maps parent -> body, so the inverse re-expresses I_a in parent coordinates (eq. 7.47).
            I_A[parent] = I_A.at(parent) + i_X_p.at(body).getInverse().transformSpatialInertia(I_a);
            p_A[parent] = p_A.at(parent) + i_X_p.at(body).getInverse() * p_a;
        }
    }

    // third pass: outward, root to tip. Calculate accelerations.
    //
    // Gravity is applied via Featherstone's trick (pp. 94): giving the base an acceleration of
    // -a_g makes every body's computed acceleration carry the gravitational term, with no explicit
    // body forces
    a[base] = SpatialVelocity(Vector3(), -getConstantGravity());

    std::vector<double> alpha_dot { };
    for (size_t i = 1; i < _bodies.size(); ++i)
    {
        Body* body = _bodies[i].get();
        Joint* joint = body->getJoint();
        const Body* parent = joint->getInfo().parent;
        Matrix S = joint->getMotionSubspace();

        SpatialVelocity a_prime = (i_X_p.at(body) * a.at(parent)) + c.at(body);
        Vector alpha_dot_i = D_inv.at(body) * (u.at(body) - U.at(body).getTranspose() * a_prime.getVector());
        append(alpha_dot, alpha_dot_i);

        a[body] = a_prime + SpatialVelocity(S * alpha_dot_i);
    }

    return toVector(alpha_dot);
}

void StellariumSimulation::updateFrames()
{
    // Forward kinematics. Recomputes the same i_X_0 / v chain as the first pass of the ABA, but
    // writes the result out to each Body's Frames in the conventions the render layer reads.
    std::map<const Body*, SpatialTransform> i_X_0 { };
    std::map<const Body*, SpatialVelocity> v { };

    const Body* base = getBase();
    i_X_0[base] = SpatialTransform();
    v[base] = SpatialVelocity();

    for (size_t i = 1; i < _bodies.size(); ++i)
    {
        Body* body = _bodies[i].get();
        Joint* joint = body->getJoint();
        if (!joint)
        {
            throw std::runtime_error("Body '" + body->getName() + "' has no joint. Only the root body may be jointless.");
        }
        const Body* parent = joint->getInfo().parent;

        SpatialTransform i_X_p = joint->getInfo().child_to_joint.getInverse() * joint->getJointTransform() * joint->getInfo().parent_to_joint;

        i_X_0[body] = i_X_p * i_X_0.at(parent);
        v[body] = i_X_p * v.at(parent) + joint->getJointVelocity();

        body->setPoseFromBase(i_X_0.at(body), v.at(body));
    }
}

void StellariumSimulation::run(double t)
{

    const double t_f = time() + t;

    // Make sure the render thread sees correct poses before the first physics step lands.
    updateFrames();

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
    // Tear the graphics down first: its Models hold raw Frame* pointers into the bodies, so the
    // bodies have to outlive it.
#ifdef STELL_BUILD_RENDERING
    _graphics.reset();
#endif
    _bodies.clear();
}

} // end namespace Stellarium
