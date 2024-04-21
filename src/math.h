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
        ~Vector3() {};

        double x() const { return _x; };
        double y() const { return _y; };
        double z() const { return _z; };

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
        Vector3 operator*(double c) { return Vector3(_x*c, _y*c, _z*c); };
        Vector3 operator/(double c) { return Vector3(_x/c, _y/c,_z/c); };
        Vector3 operator+(double c) { return Vector3(_x+c, _y+c, _z+c); };
        Vector3 operator+(Vector3 v) { return Vector3(_x+v[0], _y+v[1], _z+v[2]); };
        Vector3 operator/(Vector3 v) { return Vector3(_x/v[0], _y/v[1], _z/v[2]); };

        double dot(const Vector3& v) { return _x*v[0] + _y*v[1] + _z*v[2]; };

        Vector3 cross(const Vector3& v) {
            return Vector3(
                _y*v[2] - _z*v[1],
                _z*v[0] - _x*v[2],
                _x*v[1] - _y*v[0]
            );
        };

        double norm() { return std::sqrt(pow(_x, 2) + pow(_y, 2) + pow(_z, 2)); };

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
        Quaternion(double x, double y, double z, double w) : _x(x), _y(y), _z(z), _w(w) {};
        ~Quaternion() {};

        double x() const { return _x; };
        double y() const { return _y; };
        double z() const { return _z; };
        double w() const { return _w; };

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
            if (idx == 3)
            {
                return _w;
            }
            throw std::invalid_argument("invalid index"); 
        }

        Quaternion operator*(Quaternion q) {
            return Quaternion(
                _x*q[0] - _y*q[1] - _z*q[2] - _w*q[3],
                _x*q[1] + _y*q[0] - _z*q[3] + _w*q[2],
                _x*q[2] + _y*q[3] + _z*q[0] - _w*q[1],
                _x*q[3] - _y*q[2] + _z*q[1] + _w*q[0]
            );
        }

        Vector3 operator*(Vector3 v) { 
            // passive rotation
            Quaternion p = Quaternion(v[0], v[1], v[2], 0.0);
            Quaternion pp = *this * p * this->inverse();
            return Vector3(pp[0], pp[1], pp[2]);
        };

        Quaternion inverse() { return Quaternion(-_x, -_y, -_z, _w); };

    protected:
        double _x = 0.0;
        double _y = 0.0;
        double _z = 0.0;
        double _w = 1.0;

    private:
};


class Matrix33
{    
    public:
        Matrix33();
        Matrix33(Vector3 x, Vector3 y, Vector3 z) : _x(x), _y(y), _z(z) {};
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

        Vector3 operator*(Vector3 v) { 
            return Vector3(
                _x.dot(v),
                _y.dot(v),
                _z.dot(v)
            );
        };


    protected:
        Vector3 _x {0.0, 0.0, 0.0}; // row 1
        Vector3 _y {0.0, 0.0, 0.0}; // row 2
        Vector3 _z {0.0, 0.0, 0.0}; // row 3

    private:
};



} // end namespace Stellarium

#endif // end STELL_MATH