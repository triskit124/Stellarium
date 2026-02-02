#include "Camera.h"
#include "Matrix44.h"
#include "Quaternion.h"
#include "Vector4.h"


namespace Stellarium
{

Matrix44 Camera::getProjectionMatrix()
{
    // see: https://www.songho.ca/opengl/gl_projectionmatrix.html
    double tangent = tan((_fov * M_PI / 180) / 2.0);
    double top =  _near_clip * tangent;
    double right = top * _aspect_ratio;
    Vector4 x { _near_clip / right, 0.0, 0.0, 0.0 };
    Vector4 y { 0.0,  _near_clip / top, 0.0, 0.0 };
    Vector4 z { 0.0, 0.0, -( _far_clip +  _near_clip) / ( _far_clip -  _near_clip), -(2 *  _far_clip *  _near_clip) / ( _far_clip -  _near_clip) };
    Vector4 w { 0.0, 0.0, -1.0, 0.0 };
    
    return Matrix44(x, y, z, w);
}

Matrix44 Camera::getViewMatrix()
{
    return getPose().getInverse().toMatrix();
}

void Camera::processKeyboardInput(Camera_Movement direction, double delta_time)
{
    double velocity = _movement_speed * delta_time;
    if (direction == FORWARD)
        _pos += _att * Vector3(0, 0, -1) * velocity; // Z points backwards in camera frame
    if (direction == BACKWARD)
        _pos -= _att * Vector3(0, 0, -1) * velocity; // Z points backwards in camera frame
    if (direction == LEFT)
        _pos -= _att * Vector3(1, 0, 0) * velocity; // X points right in camera frame
    if (direction == RIGHT)
        _pos += _att * Vector3(1, 0, 0) * velocity; // X points right in camera frame
}

void Camera::processMouseMovement(double x_offset, double y_offset, bool /* constrain_pitch = true */)
{
    x_offset *= _mouse_sensitivity;
    y_offset *= _mouse_sensitivity;

    Vector3 curr_rpy = _att.getRollPitchYaw();

    double roll = curr_rpy[0] + y_offset;
    double pitch = curr_rpy[1] - x_offset;
    double yaw = curr_rpy[2];

    setAttitude(Quaternion(roll, pitch, yaw));
}

void Camera::processMouseScroll(double y_offset )
{
    _fov += y_offset;

    if (_fov < 10)
    {
        _fov = 10;
    }
    if (_fov > 80)
    {
        _fov = 80;
    }
}

}