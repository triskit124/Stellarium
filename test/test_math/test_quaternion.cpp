#include "../../src/TestHarness.h"
#include "../../src/Math.h"

using namespace Stellarium;

int main() { 

    Test test("Quaternion");

    Quaternion q1(1, 2, -3, 4);
    Quaternion q2(5, -6, 7, -8);
    Quaternion q3(-9, 10, 11, 12);

    test.assert("operator+(Quaternion)", q1 * q2 == Quaternion(24, 48, -6, 28));

    return 0;

}

