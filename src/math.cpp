#include "math.h"

namespace Stellarium {

Vector3 operator*(double c, Vector3 v)
{
    return v*c;
};

Quaternion operator*(double c, const Quaternion& v)
{
    return v*c;
};

}