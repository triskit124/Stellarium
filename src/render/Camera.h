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

        explicit Camera(const std::string& name, const Vector3& pos = Vector3(0, 0, 0), const Quaternion& att = Quaternion(1, 0, 0, 0), double movement_speed = 2.5, double mouse_sensitivity = 0.001, unsigned int width = 1920, unsigned int height = 1080) : Frame(name, pos, att) {
            setMovementSpeed(movement_speed);
            setMouseSensitivity(mouse_sensitivity);
            setScreenWidth(width);
            setScreenHeight(height);
            _aspect_ratio = static_cast<double>(_screen_width) / static_cast<double>(_screen_height);
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
        * @brief Processes keyboard input to move the camera in the specified direction.
        * @param direction The direction to move the camera (FORWARD, BACKWARD, LEFT, RIGHT).
        * @param delta_time The time elapsed since the last frame, used to ensure consistent movement speed regardless of frame rate.
        */
        void processKeyboardInput(Camera_Movement direction, double delta_time);

        /**
        * @brief Processes mouse movement input to adjust the camera's orientation.
        * @param x_offset The offset in the x direction.
        * @param y_offset The offset in the y direction.
        */
        void processMouseMovement(double x_offset, double y_offset);

        /**
        * @brief Processes mouse scroll input to adjust the camera's zoom level.
        * @param y_offset The offset in the y direction (scroll amount).
        */
        void processMouseScroll(double y_offset);


    protected:

        double _movement_speed { 2.5 }; // movement speed units per second
        double _mouse_sensitivity { 0.1 }; // mouse sensitivity multiplier

        unsigned int _screen_width; // screen width in pixels
        unsigned int _screen_height; // screen height in pixels

        double _fov { 45 }; // field of view in degrees
        double _near_clip { 0.1 }; // near clip plane distance
        double _far_clip { 100 }; // far clip plane distance
        double _aspect_ratio { 1.0 }; // aspect ratio (width / height)

};

}
