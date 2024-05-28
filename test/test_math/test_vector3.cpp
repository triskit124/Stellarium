#include "../../src/TestHarness.h"
#include "../../src/Math.h"

using namespace Stellarium;

int main() { 

    Test test("Vector3");

    Vector3 v1(1, 2, -3);
    Vector3 v2(4, -5, 6);
    Vector3 v3(-7, 8, 9);

    test.assert("operator+(Vector3)", v1 + v2 == Stellarium::Vector3(5, -3, 3));
    test.assert("operator-(Vector3)", v1 - v2 == Vector3(-3, 7, -9));
    test.assert("operator+(double)", v1 + 1 == Vector3(2, 3, -2));
    test.assert("operator-(double)", v1 - 1 == Vector3(0, 1, -4));
    test.assert("operator*(double)", v1 * 2 == Vector3(2, 4, -6));
    test.assert("operator/(double)", v1 / 2 == Vector3(0.5, 1, -1.5));
    test.assert("operator==(Vector3)", v1 == Vector3(1.0, 2.0, -3.0));
    test.assert("dot(Vector3)", v1.dot(v2) == -24);
    test.assert("cross product 1", v1.cross(v2) == Vector3(-3, -18, -13));
    test.assert("cross product 2", v1.cross(v3) == Vector3(42, 12, 22));
    test.assert("cross product 3", v1.cross(v2) == Vector3(-3, -18, -13));
    test.assert("cross product identity", v1.cross(v2) == v2.cross(v1) * -1.0);
    test.assert("cross product zero", v1.cross(v1) == Vector3(0, 0, 0));

    return 0;

}

