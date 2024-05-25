#ifndef STELL_MATH
#define STELL_MATH

#include <array>
#include <cmath>
#include <math.h>
#include <stdexcept>
#include <string>

namespace Stellarium 
{


class Vector3
{    
    public:
        Vector3();
        Vector3(double x, double y, double z) : _x(x), _y(y), _z(z) {};
        Vector3(const Vector3& v) : _x(v[0]), _y(v[1]), _z(v[2]) {};
        ~Vector3() {};

        double* x() { return &_x; };
        double* y() { return &_y; };
        double* z() { return &_z; };

        double operator[](int idx) const {
            if (idx == 0)
            {
                return _x;
            }
            if (idx == 1)
            {
                return _y;
            }
            if (idx == 2)
            {
                return _z;
            }
            throw std::invalid_argument("invalid index"); 
        }
        Vector3 operator*(double c) const { return Vector3(_x*c, _y*c, _z*c); };
        Vector3 operator/(double c) const { return Vector3(_x/c, _y/c,_z/c); };
        Vector3 operator+(double c) const { return Vector3(_x+c, _y+c, _z+c); };
        Vector3 operator-(double c) const { return Vector3(_x-c, _y-c, _z-c); };
        Vector3 operator+(Vector3 v) const { return Vector3(_x+v[0], _y+v[1], _z+v[2]); };
        Vector3 operator-(Vector3 v) const { return Vector3(_x-v[0], _y-v[1], _z-v[2]); };
        void operator=(Vector3 v) { _x = v[0]; _y = v[1]; _z = v[2]; };
        void operator+=(Vector3 v) { _x += v[0]; _y += v[1]; _z += v[2]; };
        void operator-=(Vector3 v) { _x -= v[0]; _y -= v[1]; _z -= v[2]; };
        bool operator==(Vector3 v) const { return _x == v[0] && _y == v[1] && _z == v[2]; };

        double dot(const Vector3& v) const { return _x*v[0] + _y*v[1] + _z*v[2]; };

        Vector3 cross(const Vector3& v) const {
            return Vector3(
                _y*v[2] - _z*v[1],
                _z*v[0] - _x*v[2],
                _x*v[1] - _y*v[0]
            );
        };

        double norm() const { return std::sqrt(pow(_x, 2) + pow(_y, 2) + pow(_z, 2)); };

        void normalize() { 
            double n = norm();
            if (n != 1.0)
            {
                _x /= n;
                _y /= n;
                _z /= n;
            }
        }
    
    protected:
        double _x = 0.0;
        double _y = 0.0;
        double _z = 0.0;

    private:
};

class Quaternion
{    
    public:
        Quaternion();
        Quaternion(double w, double x, double y, double z) : _w(w), _x(x), _y(y), _z(z) {};
        ~Quaternion() {};

        double* w() { return &_w; };
        double* x() { return &_x; };
        double* y() { return &_y; };
        double* z() { return &_z; };

        double operator[](int idx) const {
            if (idx == 0)
            {
                return _w;
            }
            if (idx == 1)
            {
                return _x;
            }
            if (idx == 2)
            {
                return _y;
            }
            if (idx == 3)
            {
                return _z;
            }
            throw std::invalid_argument("invalid index"); 
        }

        Quaternion operator*(double c) const { return Quaternion(_x*c, _y*c, _z*c, _w*c); };
        Quaternion operator/(double c) const { return Quaternion(_x/c, _y/c, _z/c, _w/c); };
        bool operator==(Quaternion q) const { return _w == q[0] && _x == q[1] && _y == q[2] && _z == q[3]; };

        Quaternion operator*(const Quaternion& q) const {
            return Quaternion(
                // TODO check this
                _w*q[0] - _x*q[1] - _y*q[2] - _z*q[3],
                _w*q[1] + _x*q[0] - _y*q[3] + _z*q[2],
                _w*q[2] + _x*q[3] + _y*q[0] - _z*q[1],
                _w*q[3] - _x*q[2] + _y*q[1] + _z*q[0]
            );
        }

        Vector3 operator*(const Vector3& v) const { 
            // passive rotation
            // TODO check this
            Quaternion p = Quaternion(0.0, v[0], v[1], v[2]);
            Quaternion pp = *this * p * this->inverse();
            return Vector3(pp[1], pp[2], pp[3]);
        };

        Quaternion inverse() const { return Quaternion(_w, -_x, -_y, -_z); };

        double norm() const { return std::sqrt(pow(_w, 2) + pow(_x, 2) + pow(_y, 2) + pow(_z, 2)); };

    protected:
        double _w = 1.0;
        double _x = 0.0;
        double _y = 0.0;
        double _z = 0.0;

    private:
};


class Matrix33
{    
    public:
        Matrix33();
        Matrix33(Vector3 x, Vector3 y, Vector3 z) : _x(x), _y(y), _z(z) {};
        Matrix33(double x1, double x2, double x3, double y1, double y2, double y3, double z1, double z2, double z3) : _x(x1, x2, x3), _y(y1, y2, y3), _z(z1, z2, z3) {};
        ~Matrix33() {};

        Vector3 operator[](int idx) const {
            if (idx == 0)
            {
                return _x;
            }
            if (idx == 1)
            {
                return _y;
            }
            if (idx == 2)
            {
                return _z;
            }
            throw std::invalid_argument("invalid index"); 
        }

        Vector3 operator*(Vector3 v) const { 
            return Vector3(
                _x.dot(v),
                _y.dot(v),
                _z.dot(v)
            );
        };

        Matrix33 operator*(double c) const {
            return Matrix33(
                _x * c,
                _y * c,
                _z * c
            );
        };

        Matrix33 operator/(double c) const {
            return Matrix33(
                _x / c,
                _y / c,
                _z / c
            );
        };

        double det() const { 
            return _x[0]*(_y[1]*_z[2] - _y[2]*_z[1]) 
                   - _x[1]*(_y[0]*_z[2] - _y[2]*_z[0]) 
                   + _x[2]*(_y[0]*_z[1] - _y[1]*_z[0]); 
        };

        Matrix33 transpose() const {
            return Matrix33(
                _x[0], _y[0], _z[0],
                _x[1], _y[1], _z[1],
                _x[2], _y[2], _z[2]
            );
        };

        Matrix33 adjoint() const {
            return Matrix33(
                _y[1]*_z[2] - _y[2]*_z[1],
                _x[2]*_z[1] - _x[1]*_z[2],
                _x[1]*_y[2] - _x[2]*_y[1],
                _y[2]*_z[0] - _y[0]*_z[2],
                _x[0]*_z[2] - _x[2]*_z[0],
                _x[2]*_y[0] - _x[0]*_y[2],
                _y[0]*_z[1] - _y[1]*_z[0],
                _x[1]*_z[0] - _x[0]*_z[1],
                _x[0]*_y[1] - _x[1]*_y[0]
            );
        };

        Matrix33 inverse() const {
            return adjoint().transpose() / det();
        };


    protected:
        Vector3 _x {0.0, 0.0, 0.0}; // row 1
        Vector3 _y {0.0, 0.0, 0.0}; // row 2
        Vector3 _z {0.0, 0.0, 0.0}; // row 3

    private:
};



} // end namespace Stellarium

#endif // end STELL_MATH