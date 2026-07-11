#include "Camera.h"
#include "HomTransform.h"
#include "Matrix44.h"
#include "Quaternion.h"
#include "Vector4.h"


namespace Stellarium
{

const Quaternion Camera::WORLD_UP_TO_EYE_BASIS = Quaternion(Vector3(1, 0, 0), M_PI / 2.0);

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
    return HomTransform(getEyeOrientation(), _pos).getInverse().toMatrix();
}

void Camera::processKeyboardInput(Camera_Movement direction, double delta_time)
{
    double velocity = _movement_speed * delta_time;
    Quaternion orientation = getEyeOrientation();
    Vector3 delta;
    if (direction == FORWARD)
        delta = orientation * Vector3(0, 0, -1) * velocity; // Z points backwards in eye frame
    if (direction == BACKWARD)
        delta = orientation * Vector3(0, 0, -1) * -velocity; // Z points backwards in eye frame
    if (direction == LEFT)
        delta = orientation * Vector3(1, 0, 0) * -velocity; // X points right in eye frame
    if (direction == RIGHT)
        delta = orientation * Vector3(1, 0, 0) * velocity; // X points right in eye frame

    // Translate the camera and its orbit target together, so distance/orientation
    // (and therefore future orbit/zoom behavior) stay consistent.
    _pos += delta;
    _target += delta;
}

void Camera::orbit(double x_offset, double y_offset)
{
    x_offset *= _mouse_sensitivity;
    y_offset *= _mouse_sensitivity;

    _yaw -= x_offset;
    _pitch += y_offset;

    // Clamp pitch just short of vertical so the camera can't flip over the top.
    constexpr double kMaxPitch = M_PI_2 - 0.01;
    if (_pitch > kMaxPitch) _pitch = kMaxPitch;
    if (_pitch < -kMaxPitch) _pitch = -kMaxPitch;

    // Rebuilt fresh from persistent yaw/pitch every time (never decomposed from _att),
    // so this is immune to the Euler-angle singularities in Quaternion::getRollPitchYaw().
    //
    // _att is composed as the outer factor in getEyeOrientation() (_att * WORLD_UP_TO_EYE_BASIS),
    // so its own rotation axes act directly in world space, not eye space: yaw must therefore use
    // the true world up axis (0,0,1), while pitch uses (1,0,0) since eye/world X coincide at rest.
    setAttitude(Quaternion(Vector3(0, 0, 1), _yaw) * Quaternion(Vector3(1, 0, 0), _pitch));
    updatePositionFromOrbit();
}

void Camera::pan(double x_offset, double y_offset)
{
    // Scale by distance so panning feels consistent whether zoomed in or out, matching Blender.
    double scale = _pan_speed * _distance;
    Quaternion orientation = getEyeOrientation();
    Vector3 right = orientation * Vector3(1, 0, 0);
    Vector3 up = orientation * Vector3(0, 1, 0);

    Vector3 delta = (right * -x_offset + up * y_offset) * scale;
    _target += delta;
    _pos += delta;
}

void Camera::zoom(double y_offset)
{
    // Multiplicative dolly so zooming feels consistent at any distance, matching Blender.
    double factor = 1.0 - y_offset * _zoom_speed;
    if (factor < 0.1) factor = 0.1;
    if (factor > 10.0) factor = 10.0;
    _distance *= factor;

    if (_distance < _near_clip * 2.0)
    {
        _distance = _near_clip * 2.0;
    }

    updatePositionFromOrbit();
}

}