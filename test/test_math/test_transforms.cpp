#include "Matrix33.h"
#include "TestHarness.h"
#include "Matrix44.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "Frame.h"

using namespace Stellarium;


int main() {

    Test test("Homogeneous Transform Matrix");

    /*
    =====================================================================
            Homogeneous Transform Matrix (Matrix44) Test
    =====================================================================
    */

    // Transform representation of point P from Frame_A to Frame_B
    // The Frame transformation from Frame_A to Frame_B is:
    //   - rotate frame by -90 degrees about x-axis w.r.t Frame_A expressed in Frame_A
    //   - followed by translation of [1, 2, -3] w.r.t Frame_A expressed in Frame_A
    Quaternion q_a2b = Quaternion(Vector3(1, 0, 0), -M_PI/2);
    Vector3 r_a2b = Vector3(1, 2, -3);

    Frame frame_a = Frame("Frame_A");
    Frame frame_b = Frame("Frame_B", r_a2b, q_a2b);

    test.assertTrue("Homtran from A to B - trans portion", frame_a.getTransformTo(frame_b).getTranslation() == r_a2b);
    test.assertTrue("Homtran from A to B - rot portion", frame_a.getTransformTo(frame_b).getRotation() == q_a2b.getRotationMatrix());
    test.assertTrue("Homtran from A to B", frame_a.getTransformTo(frame_b) == Matrix44(q_a2b, r_a2b));

    Vector3 p_a = Vector3(0,1,0); // Point P in Frame_A
    
    // Get point P in Frame_B [frame rotation only]
    Matrix44 T_a2b = Matrix44(q_a2b);
    Vector3 p_b = T_a2b.getInverseTransform() * p_a;
    test.assertTrue("Homtran, rot only",  p_b == Vector3(0, 0, 1));

    // Get point P in Frame_B [frame translation only]
    T_a2b = Matrix44(Quaternion(1, 0, 0, 0), r_a2b);
    p_b = T_a2b.getInverseTransform() * p_a;
    test.assertTrue("Homtran, trans only", p_b == Vector3(-1, -1, 3));
    
    // Get point P in Frame_B [rotation and translation]
    T_a2b = frame_a.getTransformTo(frame_b);
    p_b = T_a2b.getInverseTransform() * p_a;
    test.assertTrue("Homtran, rot and trans", p_b == Vector3(-1, -3, -1));

    // Test inverse transforms
    test.assertTrue("Inverse transform 1 ", frame_a.getTransformTo(frame_b) * frame_a.getTransformTo(frame_b).getInverseTransform() == Matrix44(Quaternion(1, 0, 0, 0), Vector3(0, 0, 0)));
    test.assertTrue("Inverse transform 2 ", frame_a.getTransformTo(frame_b) == frame_b.getTransformTo(frame_a).getInverseTransform());
    test.assertTrue("Inverse transform 3 ", frame_b.getTransformTo(frame_a) == frame_a.getTransformTo(frame_b).getInverseTransform());

    return 0;
}