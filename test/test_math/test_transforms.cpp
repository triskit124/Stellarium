#include "TestHarness.h"
#include "SpatialTransform.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Frame.h"

using namespace Stellarium;


int main() {

    Test test("Spatial Transform (frame poses and point transforms)");

    /*
    =====================================================================
            Frame poses / point transforms via SpatialTransform
    =====================================================================
    */

    // Transform representation of point P from Frame_A to Frame_B
    // The Frame transformation from Frame_A to Frame_B is:
    //   - rotate frame by -90 degrees about x-axis
    //   - followed by translation of [1, 2, -3] w.r.t Frame_A expressed in Frame_A
    Quaternion q_a2b = Quaternion(Vector3(1, 0, 0), -M_PI/2);
    Vector3 r_a2b = Vector3(1, 2, -3);

    Frame frame_a = Frame("Frame_A");
    Frame frame_b = Frame("Frame_B", r_a2b, q_a2b);

    test.assertTrue("Transform from A to B - trans portion", frame_a.getTransformTo(frame_b).getTranslation() == r_a2b);
    test.assertTrue("Transform from A to B - rot portion", frame_a.getTransformTo(frame_b).getRotation() == q_a2b);
    test.assertTrue("Transform from A to B", frame_a.getTransformTo(frame_b) == SpatialTransform(q_a2b, r_a2b));

    Vector3 p_a = Vector3(0,1,0); // Point P in Frame_A

    // Get point P in Frame_B [frame rotation only]
    SpatialTransform B_X_A = SpatialTransform(q_a2b, Vector3());
    Vector3 p_b = B_X_A * p_a;
    test.assertTrue("Point transform, rot only",  p_b == Vector3(0, 0, 1));

    // Get point P in Frame_B [frame translation only]
    B_X_A = SpatialTransform(Quaternion(), r_a2b);
    p_b = B_X_A * p_a;
    test.assertTrue("Point transform, trans only", p_b == Vector3(-1, -1, 3));

    // Get point P in Frame_B [rotation and translation]
    B_X_A = frame_a.getTransformTo(frame_b);
    p_b = B_X_A * p_a;
    test.assertTrue("Point transform, rot and trans", p_b == Vector3(-1, -3, -1));

    // The homogeneous matrix (used by the render layer) must agree with transformPoint()
    Vector p_b_h = B_X_A.getHomogeneousMatrix() * Vector4(p_a[0], p_a[1], p_a[2], 1.0);
    test.assertTrue("Homogeneous matrix agrees with transformPoint",
                    Vector3(p_b_h[0], p_b_h[1], p_b_h[2]) == p_b && std::abs(p_b_h[3] - 1.0) <= 1e-12);

    // The inverse maps B points back to A points
    test.assertTrue("Inverse point transform", B_X_A.getInverse() * p_b == p_a);

    // Test inverse transforms
    test.assertTrue("Inverse transform 1 ", frame_a.getTransformTo(frame_b) * frame_a.getTransformTo(frame_b).getInverse() == SpatialTransform());
    test.assertTrue("Inverse transform 2 ", frame_a.getTransformTo(frame_b) == frame_b.getTransformTo(frame_a).getInverse());
    test.assertTrue("Inverse transform 3 ", frame_b.getTransformTo(frame_a) == frame_a.getTransformTo(frame_b).getInverse());

    return test.getNumFails();
}
