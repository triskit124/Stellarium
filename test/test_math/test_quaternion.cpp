#include <cmath>
#include <iostream>
#include <math.h>

#include "TestHarness.h"
#include "Quaternion.h"
#include "Vector3.h"

using namespace Stellarium;


/*
Simple test for the Quaternion class.
Exercises the operators and math methods.

References:
    [1] https://math.umd.edu/~immortal/MATH431/book/ch_quaternions.pdf
    [2] https://faculty.sites.iastate.edu/jia/files/inline-files/quaternion.pdf

*/
int main() { 

    Test test("Quaternion");

    /*
    ========================================
            test basic operations
    ========================================
    */

    Quaternion qq(1, 2, 3, 4, false);
    test.assertTrue("Quaternion operators 1", 2*qq == Quaternion(2, 4, 6, 8, false));
    test.assertTrue("Quaternion operators 2", qq*2 == Quaternion(2, 4, 6, 8, false));
    test.assertTrue("Quaternion operators 3", qq/2 == Quaternion(0.5, 1, 1.5, 2, false));

    /* 
    ==============================================================================================
            test quaternion multiplation (Hamilton product), conjugation, norm, inverse
    ==============================================================================================
    */

    // ref [1] Example 1.1
    Quaternion q1(2, 1, 0, 0);
    Quaternion q2(3, 0, 4, 0);
    test.assertTrue("Quaternion multiplication 1", q1*q2 == Quaternion(6, 3, 8, 4));
    test.assertTrue("Quaternion multiplication 2", q2*q1 == Quaternion(6, 3, 8, -4));

    // ref [1] Example 1.2
    q1 = Quaternion(2, 3, -2, 1);
    q2 = Quaternion(1, -1, 4, 5);
    test.assertTrue("Quaternion multiplication 3", q1*q2 == Quaternion(8, -13, -10, 21));

    // ref [2] Example 1
    Quaternion p = Quaternion(3, 1, -2, 1);
    Quaternion q = Quaternion(2, -1, 2, 3);
    test.assertTrue("Quaternion multiplication 4", p*q == Quaternion(8, -9, -2, 11));
    
    // ref [1] Definition 2.3.1
    test.assertTrue("Quaternion conjugation 1", q1.getConjugate() == Quaternion(2, -3, 2, -1));
    test.assertTrue("Quaternion conjugation 2", q2.getConjugate() == Quaternion(1, 1, -4, -5));
    
    // ref [1] Theorem 2.4.2
    test.assertTrue("Quaternion conjugation property 1", q1 * q1.getConjugate() == std::pow(q1.getNorm(), 2));
    test.assertTrue("Quaternion conjugation property 2", q2 * q2.getConjugate() == std::pow(q2.getNorm(), 2));

    // ref [1] Theorem 2.6.1
    test.assertTrue("Quaternion inverse 1", q1.getInverse()*q1 == Quaternion(1, 0, 0, 0));
    test.assertTrue("Quaternion inverse 2", q2.getInverse()*q2 == Quaternion(1, 0, 0, 0));

    /* 
    =========================================================
            test vector rotation with quaternions
    =========================================================
    */

    // rotate vector by +90 degrees about z-axis
    q1 = Quaternion(Vector3(0, 0, 1), M_PI/2);
    test.assertTrue("Quaternion to rotation matrix 1", q1.getRotationMatrix() == Matrix33(Vector3(0, -1, 0), Vector3(1, 0, 0), Vector3(0, 0, 1)));
    test.assertTrue("Quaternion rotation +90 z", q1 * Vector3(1,0,0) == Vector3(0, 1, 0));

    // rotate vector by -90 degrees about z-axis
    q1 = Quaternion(Vector3(0, 0, 1), -M_PI/2);
    test.assertTrue("Quaternion to rotation matrix 2", q1.getRotationMatrix() == Matrix33(Vector3(0, 1, 0), Vector3(-1, 0, 0), Vector3(0, 0, 1)));
    test.assertTrue("Quaternion rotation -90 z", q1 * Vector3(1,0,0) == Vector3(0, -1, 0));

    // rotate vector by +90 degrees about y-axis
    q1 = Quaternion(Vector3(0, 1, 0), M_PI/2);
    test.assertTrue("Quaternion to rotation matrix 3", q1.getRotationMatrix() == Matrix33(Vector3(0, 0, 1), Vector3(0, 1, 0), Vector3(-1, 0, 0)));
    test.assertTrue("Quaternion rotation +90 y", q1 * Vector3(1,0,0) == Vector3(0, 0, -1));

    // rotate vector by -90 degrees about y-axis
    q1 = Quaternion(Vector3(0, 1, 0), -M_PI/2);
    test.assertTrue("Quaternion to rotation matrix 4", q1.getRotationMatrix() == Matrix33(Vector3(0, 0, -1), Vector3(0, 1, 0), Vector3(1, 0, 0)));
    test.assertTrue("Quaternion rotation -90 y", q1 * Vector3(1,0,0) == Vector3(0, 0, 1));

    // rotate vector by +90 degrees about x-axis
    q1 = Quaternion(Vector3(1, 0, 0), M_PI/2);
    test.assertTrue("Quaternion to rotation matrix 5", q1.getRotationMatrix() == Matrix33(Vector3(1, 0, 0), Vector3(0, 0, -1), Vector3(0, 1, 0)));
    test.assertTrue("Quaternion rotation +90 x", q1 * Vector3(0,1,0) == Vector3(0, 0, 1));

    // rotate vector by -90 degrees about x-axis
    q1 = Quaternion(Vector3(1, 0, 0), -M_PI/2);
    test.assertTrue("Quaternion to rotation matrix 6", q1.getRotationMatrix() == Matrix33(Vector3(1, 0, 0), Vector3(0, 0, 1), Vector3(0, -1, 0)));
    test.assertTrue("Quaternion rotation -90 x", q1 * Vector3(0,1,0) == Vector3(0, 0, -1));

    // ref [1] Example 5.1
    q1 = Quaternion(std::sqrt(3)/2, 0, 1/(2*std::sqrt(2)), 1/(2*std::sqrt(2)));
    Vector3 u = Vector3(2, 1, 0);
    
    Vector3 u1 = q1 * u;
    u1.setEpsilon(1e-5);
    
    Vector3 u2 = Vector3(0.3876, 1.9747, -0.9747).getNormalized();
    test.assertTrue("Quaternion rotation complicated", u1 == u2);

    return 0;

}

