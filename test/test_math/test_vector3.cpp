#include "../../src/TestHarness.h"
#include "../../src/Quaternion.h"
#include "../../src/Vector3.h"

using namespace Stellarium;

/*
Simple test for the Vector3 class.
Exercises the operators and math methods.
*/

int main() { 

    Test test("Vector3");

    Vector3 v1(1, 2, -3);
    Vector3 v2(4, -5, 6);
    Vector3 v3(-7, 8, 9);

    /* 
    ===========================
            test operators
    ===========================
    */
    test.assert("operator+(Vector3)", v1 + v2 == Stellarium::Vector3(5, -3, 3));
    test.assert("operator-(Vector3)", v1 - v2 == Vector3(-3, 7, -9));
    test.assert("operator+(double) rhs", v1 + 1 == Vector3(2, 3, -2));
    test.assert("operator-(double) rhs", v1 - 1 == Vector3(0, 1, -4));
    test.assert("operator*(double) rhs", v1 * 2 == Vector3(2, 4, -6));
    test.assert("operator/(double) rhs", v1 / 2 == Vector3(0.5, 1, -1.5));
    test.assert("operator+(double) lhs", 1 + v1 == Vector3(2, 3, -2));
    test.assert("operator-(double) lhs", -1 + v1 == Vector3(0, 1, -4));
    test.assert("operator*(double) lhs", 2 * v1 == Vector3(2, 4, -6));
    test.assert("operator==(Vector3)", v1 == Vector3(1.0, 2.0, -3.0));
    
    /* 
    ===============================
            test math methods
    ===============================
    */
    test.assert("dot(Vector3)", v1.dot(v2) == -24);
    test.assert("cross product 1", v1.cross(v2) == Vector3(-3, -18, -13));
    test.assert("cross product 2", v1.cross(v3) == Vector3(42, 12, 22));
    test.assert("cross product 3", v1.cross(v2) == Vector3(-3, -18, -13));
    test.assert("cross product identity", v1.cross(v2) == -v2.cross(v1));
    test.assert("cross product zero 1", v1.cross(v1) == Vector3(0, 0, 0));
    test.assert("cross product zero 2", v2.cross(v2) == Vector3(0, 0, 0));
    test.assert("norm 1", v1.norm() == sqrt(14));
    test.assert("norm 2", v2.norm() == sqrt(77));
    test.assert("norm 3", v3.norm() == sqrt(194));
    
    v1.normalize();
    v2.normalize();
    test.assert("normalize 1", v1 == Vector3(1/sqrt(14), 2/sqrt(14), -3/sqrt(14)));
    test.assert("normalize 2", v1.isUnit());
    test.assert("normalize 3", v2 == Vector3(4/sqrt(77), -5/sqrt(77), 6/sqrt(77)));
    test.assert("normalize 4", v2.isUnit());

    return 0;
}

