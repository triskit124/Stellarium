#pragma once

#include "Frame.h"
#include "Matrix44.h"
#include "Vector3.h"
#include <cmath>

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

        Camera(const std::string& name, const Vector3& pos = Vector3(0, 0, 0), const Quaternion& att = Quaternion(1, 0, 0, 0), double movement_speed = 2.5, double mouse_sensitivity = 0.001, unsigned int width = 1920, unsigned int height = 1080) : Frame(name, pos, att) {
            setMovementSpeed(movement_speed);
            setMouseSensitivity(mouse_sensitivity);
            setScreenWidth(width);
            setScreenHeight(height);
            _aspect_ratio = static_cast<double>(_screen_width) / static_cast<double>(_screen_height);
        } ;

        void setMovementSpeed(double speed) { _movement_speed = speed; };
        double getMovementSpeed() { return _movement_speed; };

        void setMouseSensitivity(double sensitivity) { _mouse_sensitivity = sensitivity; };
        double getMouseSensitivity() { return _mouse_sensitivity; };

        void setScreenWidth(unsigned int width) { _screen_width = width; };
        unsigned int getScreenWidth() { return _screen_width; };

        void setScreenHeight(unsigned int height) { _screen_height = height; };
        unsigned int getScreenHeight() { return _screen_height; };

        Matrix44 getProjectionMatrix();

        Matrix44 getViewMatrix();

        void processKeyboardInput(Camera_Movement direction, double delta_time);

        void processMouseMovement(double x_offset, double y_offset, bool constrain_pitch = true);

        void processMouseScroll(double y_offset);


    protected:

        double _movement_speed { 2.5 };
        double _mouse_sensitivity { 0.1 };

        unsigned int _screen_width;
        unsigned int _screen_height;

        double _fov { 45 }; // field of view in degrees
        double _near_clip { 0.1 };
        double _far_clip { 100 };
        double _aspect_ratio { 1.0 };

};

}
