
#include "TestHarness.h"
#include "Matrix33.h"

using namespace Stellarium;

int main() {
    
    Test test("Matrix33");
    
    /* 
    =======================
            matrix 1
    =======================
    */
    Matrix33 M1 = Matrix33(Vector3(7, 2, 1), Vector3(0, 3, -1), Vector3(-3, 4, -2));
    
    // Determinant
    test.assertTrue("Matrix33 determinant 1", M1.getDeterminant() == 1);

    // Transpose
    test.assertTrue("Matrix33 transpose 1", M1.getTranspose() == Matrix33(Vector3(7, 0, -3), Vector3(2, 3, 4), Vector3(1, -1, -2)));

    // Adjugate
    test.assertTrue("Matrix33 adjugate 1", M1.getAdjugate() == Matrix33(Vector3(-2, 8, -5), Vector3(3, -11, 7), Vector3(9, -34, 21)));

    // Inverse
    test.assertTrue("Matrix33 inverse 1", M1.getInverse() == Matrix33(Vector3(-2, 8, -5), Vector3(3, -11, 7), Vector3(9, -34, 21)));

    /* 
    =======================
            matrix 2
    =======================
    */
    Matrix33 M2 = Matrix33(Vector3(1, 2, 3), Vector3(3, 2, 1), Vector3(2, 1, 3));
    
    // Determinant
    test.assertTrue("Matrix33 determinant 2", M2.getDeterminant() == -12);

    // Transpose
    test.assertTrue("Matrix33 transpose 2", M2.getTranspose() == Matrix33(Vector3(1, 3, 2), Vector3(2, 2, 1), Vector3(3, 1, 3)));

    // Adjugate
    test.assertTrue("Matrix33 adjugate 2", M2.getAdjugate() == Matrix33(Vector3(5, -3, -4), Vector3(-7, -3, 8), Vector3(-1, 3, -4)));

    // Inverse
    test.assertTrue("Matrix33 inverse 2", M2.getInverse() == Matrix33(Vector3(-5, 3, 4), Vector3(7, 3, -8), Vector3(1, -3, 4)) / 12);

    /*
    =======================
            skew
    =======================
    */
    Vector3 v1(1, 2, -3);
    Vector3 v2(4, -5, 6);

    // Basic construction
    test.assertTrue("Matrix33 skew construction", Matrix33::skew(Vector3(1, 2, 3)) == Matrix33(Vector3(0, -3, 2), Vector3(3, 0, -1), Vector3(-2, 1, 0)));

    // Cross-product identity: skew(v) * u == v.cross(u)
    test.assertTrue("Matrix33 skew cross product identity", Matrix33::skew(v1) * v2 == v1.cross(v2));

    // Skew-symmetry: transpose is the negation
    test.assertTrue("Matrix33 skew is skew-symmetric", Matrix33::skew(v1).getTranspose() == Matrix33::skew(v1) * -1);

    // Zero vector maps to the zero matrix
    test.assertTrue("Matrix33 skew of zero vector", Matrix33::skew(Vector3(0, 0, 0)) == Matrix33(Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(0, 0, 0)));

    return test.getNumFails();
}