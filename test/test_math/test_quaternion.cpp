#include "../../src/TestHarness.h"
#include "../../src/Math.h"
#include <cmath>
#include <math.h>

using namespace Stellarium;

int main() { 

    Test test("Quaternion");

    Quaternion q1(2, 1, 0, 0, false);
    Quaternion q2(3, 0, 4, 0, false);

    test.assert("Quaternion multiplication 1", q1*q2 == Quaternion(6, 3, 8, 4));
    test.assert("Quaternion multiplication 2", q2*q1 == Quaternion(6, 3, 8, -4));

    q1 = Quaternion(2, 3, -2, 1, false);
    q2 = Quaternion(1, -1, 4, 5, false);

    test.assert("Quaternion multiplication 3", q1*q2 == Quaternion(8, -13, -10, 21));
    
    test.assert("Quaternion conjugation 1", q1.conjugate() == Quaternion(2, -3, 2, -1));
    test.assert("Quaternion conjugation 2", q2.conjugate() == Quaternion(1, 1, -4, -5));
    
    test.assert("Quaternion conjugation property 1", q1*q1.conjugate() == Quaternion(18, 0, 0, 0));
    test.assert("Quaternion conjugation property 2", q2.conjugate()*q2 == Quaternion(43, 0, 0, 0));

    test.assert("Quaternion inverse 1", q1.inverse()*q1 == Quaternion(1, 0, 0, 0));
    test.assert("Quaternion inverse 2", q2.inverse()*q2 == Quaternion(1, 0, 0, 0));

    // test quaterion rotations

    q1 = Quaternion(0.5, 0.5, 0.5, 0.5);
    Vector3 u = Vector3(1, 0, 0);

    test.assert("Quaternion rotation 1", q1 * u == Vector3(0, 1, 0));

    q1 = Quaternion(std::sqrt(3)/2, 1/(2*std::sqrt(2)), 1/(2*std::sqrt(2)), 0);
    u = Vector3(2, 1, 0);

    return 0;

}

