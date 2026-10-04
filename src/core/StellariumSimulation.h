#pragma once

#include "Body.h"
#include "Joint.h"
#include "SpatialInertia.h"
#include "Vector.h"
#include "Vector3.h"
#include "InertiaMatrix.h"
#include "Quaternion.h"
#include "Integrators.h"

#ifdef STELL_BUILD_RENDERING
#include "GraphicsEngine.h"
#include "Model.h"
#endif

#include <memory>
#include <vector>


namespace Stellarium
{

/**
 * @brief The StellariumSimulation class
 */
class StellariumSimulation
{
    public:
        /**
         * @brief Constructs a StellariumSimulation object.
         */
        StellariumSimulation();

        /**
         * @brief Destroys the StellariumSimulation object.
         */
        ~StellariumSimulation();

        /**
         * @brief Initializes the graphics engine.
         */
        void addGraphics();

        /**
         * @brief Adds a body to the simulation.
         *
         * @param name The name of the body.
         * @param spatial_inertia The spatial inertia (mass, center of mass, inertia tensor) of the body.
         * @param joint_info The joint connecting this body to its predecessor.
         */
        Body* addBody(const std::string& name,
                      const SpatialInertia& spatial_inertia,
                      const Joint::Info& joint_info
        );

        /**
         * @brief Adds an integrator to the simulation.
         *
         * @param type The type of the integrator.
         * @param dt The size of the fixed integrator step.
         */
        void addIntegrator(const STELL_INTEGRATOR_TYPE& type,  const double dt);

        /**
         * @brief Returns the current time of the simulation.
         */
        double time() const { return _t; };

        /**
         * @brief Runs the simulation for a given time.
         *
         * @param t The time to run the simulation for.
         */
        void run(double t);

        void addConstantGravity(const Vector3& g) { _gravity = g; };
        Vector3 getConstantGravity() const { return _gravity; };

        /**
         * @brief The fictitious root body: the fixed, inertial base of the kinematic tree.
         *
         * It carries no joint and never appears in the state vector. A body added with a null
         * Joint::Info::predecessor is attached to this body, i.e. "no predecessor" means "hung off the
         * world".
         */
        Body* getBase() { return _bodies.front().get(); };
        const Body* getBase() const { return _bodies.front().get(); };

        /**
         * @brief Runs forward dynamics at the current state and returns the generalized
         * acceleration q_ddot, concatenated over every jointed body in tree order.
         *
         * This is exactly what the integrator consumes; it is exposed so the dynamics can be
         * checked against an independent derivation without stepping the simulation.
         */
        Vector getGeneralizedAcceleration() const { return _computeForwardDynamics(); };

        /**
         * @brief Forward kinematics: turns the current joint coordinates into world poses on every
         * Body's Frames, which is what the render layer reads.
         *
         * Called automatically at the end of every step; call it directly after setting joint
         * coordinates by hand if the poses are needed before the next step.
         */
        void updateFrames();

#ifdef STELL_BUILD_RENDERING
        /**
         * @brief Loads a 3D model from a file and associates it with a frame so its transform is
         *        driven by the frame's pose each physics step.
         *
         * @param path  Path to the model file.
         * @param frame The frame whose pose drives this model's world transform.
         * @return Pointer to the stored Model.
         */
        Model* loadModel(const std::string& path, Frame& frame);

        GraphicsEngine* getGraphics() { return this->_graphics.get(); };
#endif

    protected:

    private:
        /**
        * @brief The bodies in the simulation.
        */
        std::vector<std::unique_ptr<Body>> _bodies {}; /**< vector of bodies in the simulation. */

        /**
        * @brief The integrator used for advancing the simulation.
        */
        std::unique_ptr<Integrator> _integrator = nullptr; /**< The integrator used for advancing the simulation. */

        /**
        * @brief The simulation time.
        */
        double _t = 0.0;

        Vector3 _gravity { };

#ifdef STELL_BUILD_RENDERING
        std::unique_ptr<GraphicsEngine> _graphics { };
#endif

        /**
        * @brief Advances the simulation by one step. The size of the step is determined by the integrator.
        */
        void _step();

        Vector _computeForwardDynamics() const;

        Vector _getState() const;
        Vector _getStateDot() const;
        void _setState(const Vector& s);


};

} // end namespace Stellarium
