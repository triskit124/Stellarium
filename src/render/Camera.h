#pragma once

#include "Frame.h"
#include "Matrix44.h"
#include "Vector3.h"
#include <cstddef>

namespace Stellarium
{

class Camera : public Frame
{

    public:

        enum Camera_Movement {
            FORWARD,
            BACKWARD,
            LEFT,
            RIGHT
        };

        explicit Camera(const std::string& name, const Vector3& target = Vector3(0, 0, 0), double distance = 10.0, double yaw = 0.0, double pitch = 0.0, double movement_speed = 2.5, double mouse_sensitivity = 0.005, double pan_speed = 0.002, double zoom_speed = 0.1, unsigned int width = 1920, unsigned int height = 1080)
            : Frame(name), _target(target), _distance(distance), _yaw(yaw), _pitch(pitch) {
            setMovementSpeed(movement_speed);
            setMouseSensitivity(mouse_sensitivity);
            setPanSpeed(pan_speed);
            setZoomSpeed(zoom_speed);
            setScreenWidth(width);
            setScreenHeight(height);
            _aspect_ratio = static_cast<double>(_screen_width) / static_cast<double>(_screen_height);
            setAttitude(Quaternion(Vector3(0, 1, 0), _yaw) * Quaternion(Vector3(1, 0, 0), _pitch));
            updatePositionFromOrbit();
        } ;

        /**
        * @brief Sets the movement speed of the camera. Must be positive.
        * @param speed The new movement speed.
        * @throws std::invalid_argument if speed is <= 0
        */
        void setMovementSpeed(double speed) { 
            if (speed <= 0) {
                throw std::invalid_argument("Movement speed must be positive");
            }
            _movement_speed = speed; 
        };

        /**
        * @brief Gets the movement speed of the camera.
        * @return The current movement speed.
        */
        double getMovementSpeed() { return _movement_speed; };

        /**
        * @brief Sets the mouse sensitivity of the camera.
        * @param sensitivity The new mouse sensitivity.
        * @throws std::invalid_argument if sensitivity is <= 0
        */
        void setMouseSensitivity(double sensitivity) { 
            if (sensitivity <= 0) {
                throw std::invalid_argument("Mouse sensitivity must be positive");
            }
            _mouse_sensitivity = sensitivity; 
        };

        /**
        * @brief Gets the mouse sensitivity of the camera.
        * @return The current mouse sensitivity.
        */
        double getMouseSensitivity() { return _mouse_sensitivity; };

        /**
        * @brief Sets the pan speed of the camera. Must be positive.
        * @param speed The new pan speed.
        * @throws std::invalid_argument if speed is <= 0
        */
        void setPanSpeed(double speed) {
            if (speed <= 0) {
                throw std::invalid_argument("Pan speed must be positive");
            }
            _pan_speed = speed;
        };

        /**
        * @brief Gets the pan speed of the camera.
        * @return The current pan speed.
        */
        double getPanSpeed() { return _pan_speed; };

        /**
        * @brief Sets the zoom speed of the camera. Must be positive.
        * @param speed The new zoom speed.
        * @throws std::invalid_argument if speed is <= 0
        */
        void setZoomSpeed(double speed) {
            if (speed <= 0) {
                throw std::invalid_argument("Zoom speed must be positive");
            }
            _zoom_speed = speed;
        };

        /**
        * @brief Gets the zoom speed of the camera.
        * @return The current zoom speed.
        */
        double getZoomSpeed() { return _zoom_speed; };

        /**
        * @brief Gets the point the camera orbits around.
        * @return The orbit target, in world coordinates.
        */
        Vector3 getTarget() { return _target; };

        /**
        * @brief Sets the point the camera orbits around. Re-derives the camera's position
        * to keep the current orbit distance and look angles.
        * @param target The new orbit target, in world coordinates.
        */
        void setTarget(const Vector3& target) { _target = target; updatePositionFromOrbit(); };

        /**
        * @brief Gets the camera's distance from its orbit target.
        * @return The orbit distance.
        */
        double getDistance() { return _distance; };

        /**
        * @brief Sets the screen width.
        * @param width The new screen width.
        */
        void setScreenWidth(size_t width) { _screen_width = width; };

        /**
        * @brief Gets the screen width.
        * @return The current screen width.
        */
        size_t getScreenWidth() { return _screen_width; };

        /**
        * @brief Sets the screen height.
        * @param height The new screen height.
        */
        void setScreenHeight(size_t height) { _screen_height = height; };

        /**
        * @brief Gets the screen height.
        * @return The current screen height.
        */
        size_t getScreenHeight() { return _screen_height; };

        /**
        * @brief Returns the projection matrix based on the camera's field of view, aspect ratio, and near/far clipping planes.
        * @return The projection matrix.
        */
        Matrix44 getProjectionMatrix();

        /**
        * @brief Returns the view matrix based on the camera's position and orientation.
        * @return The view matrix.
        */
        Matrix44 getViewMatrix();

        /**
        * @brief Processes keyboard input to translate the camera (and its orbit target) in the specified direction.
        * @param direction The direction to move the camera (FORWARD, BACKWARD, LEFT, RIGHT).
        * @param delta_time The time elapsed since the last frame, used to ensure consistent movement speed regardless of frame rate.
        */
        void processKeyboardInput(Camera_Movement direction, double delta_time);

        /**
        * @brief Orbits the camera around its target in response to a mouse drag (Blender-style MMB-drag).
        * @param x_offset The mouse movement in the x direction, in pixels.
        * @param y_offset The mouse movement in the y direction, in pixels.
        */
        void orbit(double x_offset, double y_offset);

        /**
        * @brief Pans the camera and its target together, perpendicular to the view direction
        * (Blender-style Shift+MMB-drag). Panning speed scales with orbit distance so it feels
        * consistent whether zoomed in or out.
        * @param x_offset The mouse movement in the x direction, in pixels.
        * @param y_offset The mouse movement in the y direction, in pixels.
        */
        void pan(double x_offset, double y_offset);

        /**
        * @brief Zooms by moving the camera closer to or further from its target (dolly), rather
        * than changing field of view (Blender-style scroll-to-zoom).
        * @param y_offset The offset in the y direction (scroll amount).
        */
        void zoom(double y_offset);


    protected:

        /**
        * @brief Returns the camera's orientation, expressed as a rotation from canonical
        * OpenGL eye axes (X-right, Y-up, -Z-forward) directly to world axes. This bakes in
        * the fixed offset between the world's Z-up/Y-forward convention and eye space, so
        * that _att alone still represents just the user-driven look rotation.
        * @return The orientation quaternion mapping eye-space vectors to world-space vectors.
        */
        Quaternion getEyeOrientation() const { return _att * WORLD_UP_TO_EYE_BASIS; };

        /**
        * @brief Recomputes _pos from _target, _distance, and the current orientation.
        * Call after anything changes orbit distance or look angles.
        */
        void updatePositionFromOrbit() { _pos = _target - (getEyeOrientation() * Vector3(0, 0, -1)) * _distance; };

        Vector3 _target { 0.0, 0.0, 0.0 }; // point the camera orbits around, in world coordinates
        double _distance { 10.0 }; // distance from _target to _pos

        double _movement_speed { 2.5 }; // movement speed units per second
        double _mouse_sensitivity { 0.1 }; // mouse sensitivity multiplier
        double _pan_speed { 0.002 }; // pan speed multiplier
        double _zoom_speed { 0.1 }; // zoom (dolly) speed multiplier

        // Persistent look angles (radians), driving _att directly so that mouse-look never
        // has to decompose an existing quaternion into Euler angles (which is what causes
        // gimbal-lock-style instability near +/-90 degrees pitch). Yaw is about the true
        // world up axis (0,0,1) and pitch about the world/eye-at-rest right axis (1,0,0) --
        // see the axis comment in Camera::orbit() for why those are world, not eye, axes.
        double _yaw { 0.0 };
        double _pitch { 0.0 };

        unsigned int _screen_width; // screen width in pixels
        unsigned int _screen_height; // screen height in pixels

        double _fov { 45 }; // field of view in degrees
        double _near_clip { 0.1 }; // near clip plane distance
        double _far_clip { 100 }; // far clip plane distance
        double _aspect_ratio { 1.0 }; // aspect ratio (width / height)

    private:

        // Fixed rotation aligning canonical eye axes (X-right, Y-up, -Z-forward) with the
        // world's Z-up, Y-forward convention: eye X -> world X, eye Y -> world Z, eye -Z -> world Y.
        static const Quaternion WORLD_UP_TO_EYE_BASIS;

};

}
