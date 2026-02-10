#include "TestHarness.h"
#include "Quaternion.h"
#include "Vector3.h"

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
    test.assertTrue("operator+(Vector3)", v1 + v2 == Vector3(5, -3, 3));
    test.assertTrue("operator-(Vector3)", v1 - v2 == Vector3(-3, 7, -9));
    test.assertTrue("operator+(double) rhs", v1 + 1 == Vector3(2, 3, -2));
    test.assertTrue("operator-(double) rhs", v1 - 1 == Vector3(0, 1, -4));
    test.assertTrue("operator*(double) rhs", v1 * 2 == Vector3(2, 4, -6));
    test.assertTrue("operator/(double) rhs", v1 / 2 == Vector3(0.5, 1, -1.5));
    test.assertTrue("operator+(double) lhs", 1 + v1 == Vector3(2, 3, -2));
    test.assertTrue("operator-(double) lhs", -1 + v1 == Vector3(0, 1, -4));
    test.assertTrue("operator*(double) lhs", 2 * v1 == Vector3(2, 4, -6));
    test.assertTrue("operator==(Vector3)", v1 == Vector3(1.0, 2.0, -3.0));
    
    /* 
    ===============================
            test math methods
    ===============================
    */
    test.assertTrue("dot(Vector3)", v1.dot(v2) == -24);
    test.assertTrue("cross product 1", v1.cross(v2) == Vector3(-3, -18, -13));
    test.assertTrue("cross product 2", v1.cross(v3) == Vector3(42, 12, 22));
    test.assertTrue("cross product 3", v1.cross(v2) == Vector3(-3, -18, -13));
    test.assertTrue("cross product identity", v1.cross(v2) == -v2.cross(v1));
    test.assertTrue("cross product zero 1", v1.cross(v1) == Vector3(0, 0, 0));
    test.assertTrue("cross product zero 2", v2.cross(v2) == Vector3(0, 0, 0));
    test.assertTrue("norm 1", v1.getNorm() == sqrt(14));
    test.assertTrue("norm 2", v2.getNorm() == sqrt(77));
    test.assertTrue("norm 3", v3.getNorm() == sqrt(194));
    
    v1.normalize();
    v2.normalize();
    test.assertTrue("normalize 1", v1 == Vector3(1/sqrt(14), 2/sqrt(14), -3/sqrt(14)));
    test.assertTrue("normalize 2", v1.isUnit());
    test.assertTrue("normalize 3", v2 == Vector3(4/sqrt(77), -5/sqrt(77), 6/sqrt(77)));
    test.assertTrue("normalize 4", v2.isUnit());

    return test.getNumFails();
}

