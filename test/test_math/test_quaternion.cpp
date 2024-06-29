#include "../../src/TestHarness.h"
#include "../../src/Math.h"
#include <cmath>
#include <iostream>
#include <math.h>

using namespace Stellarium;


/*
Simple test for the Quaternion class.
Exercises the operators and math methods.

Reference:
    - https://math.umd.edu/~immortal/MATH431/book/ch_quaternions.pdf

*/
int main() { 

    Test test("Quaternion");

    /* 
    ==============================================================================================
            test quaternion multiplation (Hamilton product), conjugation, norm, inverse
    ==============================================================================================
    */

    // Example 1.1
    Quaternion q1(2, 1, 0, 0);
    Quaternion q2(3, 0, 4, 0);
    test.assert("Quaternion multiplication 1", q1*q2 == Quaternion(6, 3, 8, 4));
    test.assert("Quaternion multiplication 2", q2*q1 == Quaternion(6, 3, 8, -4));

    // Example 1.2
    q1 = Quaternion(2, 3, -2, 1);
    q2 = Quaternion(1, -1, 4, 5);
    test.assert("Quaternion multiplication 3", q1*q2 == Quaternion(8, -13, -10, 21));
    
    // Definition 2.3.1
    test.assert("Quaternion conjugation 1", q1.conjugate() == Quaternion(2, -3, 2, -1));
    test.assert("Quaternion conjugation 2", q2.conjugate() == Quaternion(1, 1, -4, -5));
    
    // Theorem 2.4.2
    test.assert("Quaternion conjugation property 1", q1 * q1.conjugate() == std::pow(q1.norm(), 2));
    test.assert("Quaternion conjugation property 2", q2 * q2.conjugate() == std::pow(q2.norm(), 2));

    // Theorem 2.6.1
    test.assert("Quaternion inverse 1", q1.inverse()*q1 == Quaternion(1, 0, 0, 0));
    test.assert("Quaternion inverse 2", q2.inverse()*q2 == Quaternion(1, 0, 0, 0));

    /* 
    =========================================================
            test vector rotation with quaternions
    =========================================================
    */

    // rotate unit x-vector by +90 degrees about z-axis 
    test.assert("Quaternion rotation +90 z", Quaternion(Vector3(0, 0, 1), M_PI/2) * Vector3(1,0,0) == Vector3(0, 1, 0));

    // rotate unit x-vector by -90 degrees about z-axis
    test.assert("Quaternion rotation -90 z", Quaternion(Vector3(0, 0, 1), -M_PI/2) * Vector3(1,0,0) == Vector3(0, -1, 0));

    // rotate unit x-vector by +90 degrees about y-axis
    test.assert("Quaternion rotation +90 y", Quaternion(Vector3(0, 1, 0), M_PI/2) * Vector3(1,0,0) == Vector3(0, 0, -1));

    // rotate unit x-vector by -90 degrees about y-axis
    test.assert("Quaternion rotation -90 y", Quaternion(Vector3(0, 1, 0), -M_PI/2) * Vector3(1,0,0) == Vector3(0, 0, 1));

    // rotate unit y-vector by +90 degrees about x-axis
    test.assert("Quaternion rotation +90 x", Quaternion(Vector3(1, 0, 0), M_PI/2) * Vector3(0,1,0) == Vector3(0, 0, 1));

    // rotate unit y-vector by -90 degrees about x-axis
    test.assert("Quaternion rotation -90 x", Quaternion(Vector3(1, 0, 0), -M_PI/2) * Vector3(0,1,0) == Vector3(0, 0, -1));

    // Example 5.1
    q1 = Quaternion(std::sqrt(3)/2, 0, 1/(2*std::sqrt(2)), 1/(2*std::sqrt(2)));
    Vector3 u = Vector3(2, 1, 0);
    
    Vector3 u1 = q1 * u;
    u1.epsilon(1e-5);
    
    Vector3 u2 = Vector3(0.3876, 1.9747, -0.9747).getNormalized();
    test.assert("Quaternion rotation complicated", u1 == u2);


    return 0;

}

