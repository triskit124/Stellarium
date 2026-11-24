#include <cmath>
#include <math.h>

#include "Constants.h"
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
    Quaternion q1(2, 1, 0, 0, false);
    Quaternion q2(3, 0, 4, 0, false);
    test.assertTrue("Quaternion multiplication 1", q1*q2 == Quaternion(6, 3, 8, 4, false));
    test.assertTrue("Quaternion multiplication 2", q2*q1 == Quaternion(6, 3, 8, -4, false));

    // ref [1] Example 1.2
    q1 = Quaternion(2, 3, -2, 1, false);
    q2 = Quaternion(1, -1, 4, 5, false);
    test.assertTrue("Quaternion multiplication 3", q1*q2 == Quaternion(8, -13, -10, 21, false));

    // ref [2] Example 1
    Quaternion p = Quaternion(3, 1, -2, 1, false);
    Quaternion q = Quaternion(2, -1, 2, 3, false);
    test.assertTrue("Quaternion multiplication 4", p*q == Quaternion(8, -9, -2, 11, false));
    
    // ref [1] Definition 2.3.1
    test.assertTrue("Quaternion conjugation 1", q1.getConjugate() == Quaternion(2, -3, 2, -1, false));
    test.assertTrue("Quaternion conjugation 2", q2.getConjugate() == Quaternion(1, 1, -4, -5, false));
    
    // ref [1] Theorem 2.4.2
    test.assertTrue("Quaternion conjugation property 1", q1 * q1.getConjugate() == std::pow(q1.getNorm(), 2));
    test.assertTrue("Quaternion conjugation property 2", q2 * q2.getConjugate() == std::pow(q2.getNorm(), 2));

    // ref [1] Theorem 2.6.1
    test.assertTrue("Quaternion inverse 1", q1.getInverse()*q1 == Quaternion(1, 0, 0, 0, false));
    test.assertTrue("Quaternion inverse 2", q2.getInverse()*q2 == Quaternion(1, 0, 0, 0, false));

    /* 
    =========================================================
            test (passive) frame rotation with quaternions
    =========================================================
    */

    // rotate frame by +90 degrees about z-axis
    q1 = Quaternion(Vector3(0, 0, 1), M_PI/2);
    test.assertTrue("Quaternion to rotation matrix 1", q1.getRotationMatrix() == RotationMatrix(Vector3(0, 1, 0), Vector3(-1, 0, 0), Vector3(0, 0, 1)));
    test.assertTrue("Quaternion frame rotation +90 z", q1 * Vector3(1,0,0) == Vector3(0, -1, 0));

    // rotate frame by -90 degrees about z-axis
    q1 = Quaternion(Vector3(0, 0, 1), -M_PI/2);
    test.assertTrue("Quaternion to rotation matrix 2", q1.getRotationMatrix() == RotationMatrix(Vector3(0, -1, 0), Vector3(1, 0, 0), Vector3(0, 0, 1)));
    test.assertTrue("Quaternion frame rotation -90 z", q1 * Vector3(1,0,0) == Vector3(0, 1, 0));

    // rotate frame by +90 degrees about y-axis
    q1 = Quaternion(Vector3(0, 1, 0), M_PI/2);
    test.assertTrue("Quaternion to rotation matrix 3", q1.getRotationMatrix() == RotationMatrix(Vector3(0, 0, -1), Vector3(0, 1, 0), Vector3(1, 0, 0)));
    test.assertTrue("Quaternion frame rotation +90 y", q1 * Vector3(1,0,0) == Vector3(0, 0, 1));

    // rotate frame by -90 degrees about y-axis
    q1 = Quaternion(Vector3(0, 1, 0), -M_PI/2);
    test.assertTrue("Quaternion to rotation matrix 4", q1.getRotationMatrix() == RotationMatrix(Vector3(0, 0, 1), Vector3(0, 1, 0), Vector3(-1, 0, 0)));
    test.assertTrue("Quaternion frame rotation -90 y", q1 * Vector3(1,0,0) == Vector3(0, 0, -1));

    // rotate frame by +90 degrees about x-axis
    q1 = Quaternion(Vector3(1, 0, 0), M_PI/2);
    test.assertTrue("Quaternion to rotation matrix 5", q1.getRotationMatrix() == RotationMatrix(Vector3(1, 0, 0), Vector3(0, 0, 1), Vector3(0, -1, 0)));
    test.assertTrue("Quaternion frame rotation +90 x", q1 * Vector3(0,1,0) == Vector3(0, 0, -1));

    // rotate frame by -90 degrees about x-axis
    q1 = Quaternion(Vector3(1, 0, 0), -M_PI/2);
    test.assertTrue("Quaternion to rotation matrix 6", q1.getRotationMatrix() == RotationMatrix(Vector3(1, 0, 0), Vector3(0, 0, -1), Vector3(0, 1, 0)));
    test.assertTrue("Quaternion frame rotation -90 x", q1 * Vector3(0,1,0) == Vector3(0, 0, 1));

    // ref [1] Example 5.1
    q1 = Quaternion(std::sqrt(3)/2, 0, 1/(2*std::sqrt(2)), 1/(2*std::sqrt(2)));
    Vector3 u = Vector3(2, 1, 0);
    
    Vector3 u1 = q1.getInverse() * u; // Take inverse because example uses active rotation
    u1.setEpsilon(1e-4);
    
    Vector3 u2 = Vector3(0.3876, 1.9747, -0.9747);
    test.assertTrue("Quaternion rotation complicated", u1 == u2);

    /*
    =========================================================
            test roll-pitch-yaw round trips (passive DCM)
    =========================================================
    */

    // Each RPY quaternion is the frame rotated by roll about x, then pitch about the new y, then yaw about the new z.
    const double roll = 0.3, pitch = -0.7, yaw = 1.9;
    q1 = Quaternion(roll, pitch, yaw);
    q2 = Quaternion(Vector3(1, 0, 0), roll) * Quaternion(Vector3(0, 1, 0), pitch) * Quaternion(Vector3(0, 0, 1), yaw);
    test.assertTrue("RPY constructor composes body 1-2-3 frame rotations", q1 == q2);

    // Test round trip for rpy
    test.assertTrue("RPY -> Quat -> RotMat -> RPY 1", abs(q1.getRotationMatrix().getRollPitchYaw()[0] - roll) <= STELL_EPSILON);
    test.assertTrue("RPY -> Quat -> RotMat -> RPY 2", abs(q1.getRotationMatrix().getRollPitchYaw()[1] - pitch) <= STELL_EPSILON);
    test.assertTrue("RPY -> Quat -> RotMat -> RPY 3", abs(q1.getRotationMatrix().getRollPitchYaw()[2] - yaw) <= STELL_EPSILON);

    // Sequence check by hand: roll +90 about x, then pitch +90 about the new y
    q1 = Quaternion(M_PI/2, M_PI/2, 0.0);
    q2 = Quaternion(Vector3(1, 0, 0), M_PI/2) * Quaternion(Vector3(0, 1, 0), M_PI/2);
    test.assertTrue("RPY roll is applied before pitch 1", q1 * Vector3(1, 0, 0) == Vector3(0, 0, 1));
    test.assertTrue("RPY roll is applied before pitch 2", q2 * Vector3(1, 0, 0) == Vector3(0, 0, 1));    
    test.assertTrue("RPY roll is applied before pitch 3", q1 * Vector3(0, 1, 0) == Vector3(1, 0, 0));
    test.assertTrue("RPY roll is applied before pitch 4", q2 * Vector3(0, 1, 0) == Vector3(1, 0, 0));
    
    q1 = Quaternion(roll, pitch, yaw);

    Vector3 rpy = q1.getRollPitchYaw();
    rpy.setEpsilon(1e-12);
    test.assertTrue("Quaternion RPY round trip", rpy == Vector3(roll, pitch, yaw));
    test.assertTrue("RotationMatrix RPY agrees with Quaternion RPY", q1.getRotationMatrix().getRollPitchYaw() == q1.getRollPitchYaw());

    // Pure yaw: a frame yawed +90 deg reads world x as body -y, and RPY recovers only the yaw.
    q1 = Quaternion(0.0, 0.0, M_PI/2);
    test.assertTrue("RPY pure yaw frame rotation", q1 * Vector3(1, 0, 0) == Vector3(0, -1, 0));
    test.assertTrue("RPY pure yaw round trip", q1.getRollPitchYaw() == Vector3(0, 0, M_PI/2));

    // Gimbal lock (pitch = +/-90 deg): the angles are not unique, but they must still describe the same rotation.
    for (double gimbal_pitch : { M_PI_2, -M_PI_2 })
    {
        q1 = Quaternion(0.4, gimbal_pitch, -0.2);
        Vector3 g = q1.getRollPitchYaw();
        test.assertTrue("RPY gimbal lock pitch recovered", std::abs(g[1] - gimbal_pitch) <= 1e-9);
        test.assertTrue("RPY gimbal lock describes the same rotation",
                        Quaternion(g[0], g[1], g[2]).getRotationMatrix() == q1.getRotationMatrix());
    }

    return test.getNumFails();

}

