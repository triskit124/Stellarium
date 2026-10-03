/*
 * Pins down the spatial-algebra conventions the Featherstone implementation depends on.
 *
 * Every check here compares an operator that was written in closed form (quaternion + translation)
 * against the 6x6 matrix form derived independently from the book's definitions, so a sign or
 * transpose slip in either one shows up as a mismatch rather than as mysteriously wrong dynamics.
 *
 * Reference: Featherstone, Rigid Body Dynamics Algorithms, 2008, ch. 2.
 */

#include "Matrix.h"
#include "Matrix33.h"
#include "Quaternion.h"
#include "SpatialInertia.h"
#include "SpatialTransform.h"
#include "SpatialVector.h"
#include "SquareMatrix.h"
#include "TestHarness.h"
#include "Vector.h"
#include "Vector3.h"

#include <cmath>
#include <string>

using namespace Stellarium;

namespace {

constexpr double TOL = 1e-12;

/**
 * @brief Asserts two equally-sized vectors agree element-wise.
 */
void assertVectorEquals(Test& test, const std::string& description, const Vector& a, const Vector& b)
{
    if (a.getSize() != b.getSize()) {
        test.assertTrue(description + " (size)", false);
        return;
    }
    double worst = 0.0;
    for (size_t i = 0; i < a.getSize(); ++i) {
        worst = std::max(worst, std::abs(a[i] - b[i]));
    }
    test.assertTrue(description, worst <= TOL);
}

void assertMatrixEquals(Test& test, const std::string& description, const Matrix& a, const Matrix& b)
{
    if (a.getNumRows() != b.getNumRows() || a.getNumCols() != b.getNumCols()) {
        test.assertTrue(description + " (size)", false);
        return;
    }
    double worst = 0.0;
    for (size_t i = 0; i < a.getNumRows(); ++i) {
        for (size_t j = 0; j < a.getNumCols(); ++j) {
            worst = std::max(worst, std::abs(a[i][j] - b[i][j]));
        }
    }
    test.assertTrue(description, worst <= TOL);
}

/**
 * @brief The 6x6 force transform of eq. 2.25, built directly from its block definition
 * [ E, -E r_cross ; 0, E ] acting on [torque; force].
 */
Matrix forceMatrix(const SpatialTransform& X)
{
    Matrix E = X.getRotation().getRotationMatrix();
    Matrix r_cross = Matrix33::skew(X.getTranslation());
    Matrix zero3(3, 3);
    return (E.horizontalConcatenate(-(E * r_cross))).verticalConcatenate(zero3.horizontalConcatenate(E));
}

/**
 * @brief The spatial motion cross product operator crm(v) of eq. 2.31, in [angular; linear] order.
 */
Matrix crossMotionMatrix(const SpatialMotion& v)
{
    Matrix w_cross = Matrix33::skew(v.getAngularVelocity());
    Matrix v_cross = Matrix33::skew(v.getLinearVelocity());
    Matrix zero3(3, 3);
    return (w_cross.horizontalConcatenate(zero3)).verticalConcatenate(v_cross.horizontalConcatenate(w_cross));
}

} // end anonymous namespace


int main() {

    Test test("Spatial algebra");

    // Two arbitrary-but-fixed transforms, chosen so no block is accidentally symmetric or zero.
    const SpatialTransform X1(Quaternion(Vector3(1, 2, -3), 0.7), Vector3(0.4, -1.3, 2.1));
    const SpatialTransform X2(Quaternion(Vector3(-2, 0.5, 1), -1.1), Vector3(-0.9, 0.2, 0.6));

    const SpatialMotion v(Vector3(0.3, -1.2, 0.8), Vector3(2.0, 0.5, -1.7));
    const SpatialForce f(Vector3(-0.6, 1.1, 0.25), Vector3(0.9, -2.2, 1.4));

    /*
    ==================================================================
        Motion / force transforms agree with their 6x6 matrix forms
    ==================================================================
    */

    assertVectorEquals(test, "motion transform matches getMotionMatrix (eq. 2.24)",
                       (X1 * v).getVector(), X1.getMotionMatrix() * v.getVector());

    assertVectorEquals(test, "force transform matches its block form (eq. 2.25)",
                       (X1 * f).getVector(), forceMatrix(X1) * f.getVector());

    // The force transform is the inverse-transpose of the motion transform (eq. 2.26): this is the
    // property the ABA's inward pass leans on when it propagates p_A with a plain transpose.
    assertMatrixEquals(test, "force transform equals motion transform inverse-transpose",
                       forceMatrix(X1),
                       SquareMatrix(X1.getMotionMatrix().getTranspose()).getInverse());

    /*
    ============================
        Composition / inverse
    ============================
    */

    assertVectorEquals(test, "composition applies right operand first",
                       ((X2 * X1) * v).getVector(), (X2 * (X1 * v)).getVector());

    assertMatrixEquals(test, "composed motion matrix equals product of motion matrices",
                       (X2 * X1).getMotionMatrix(),
                       X2.getMotionMatrix() * X1.getMotionMatrix());

    assertVectorEquals(test, "inverse transform undoes the transform (motion)",
                       (X1.getInverse() * (X1 * v)).getVector(), v.getVector());

    assertVectorEquals(test, "inverse transform undoes the transform (force)",
                       (X1.getInverse() * (X1 * f)).getVector(), f.getVector());

    /*
    ==========================================
        Rotation convention: E maps P -> S
    ==========================================
        A frame rotated +90 degrees about z relative to its predecessor has its x-axis along the
        predecessor's y-axis, so a vector along the predecessor's x-axis reads (0, -1, 0) in it.
    */
    {
        const SpatialTransform X_z90(Quaternion(Vector3(0, 0, 1), M_PI / 2).getConjugate(), Vector3());
        const SpatialMotion along_x(Vector3(1, 0, 0), Vector3());
        assertVectorEquals(test, "+90 deg about z: predecessor x reads as successor -y",
                           (X_z90 * along_x).getAngularVelocity(), Vector3(0, -1, 0));
    }

    /*
    ==================================
        Pure translation (Featherstone xlt)
    ==================================
        Shifting the reference point by r leaves omega alone and subtracts r x omega from the
        linear part.
    */
    {
        const Vector3 r(0.0, 0.0, -1.0);
        const SpatialTransform X_t(Quaternion(), r);
        const SpatialMotion spin(Vector3(1, 0, 0), Vector3());
        assertVectorEquals(test, "xlt leaves angular velocity unchanged",
                           (X_t * spin).getAngularVelocity(), Vector3(1, 0, 0));
        // A unit spin about x, referenced a metre down the -z axis, gives that point a velocity of
        // omega x r = (1,0,0) x (0,0,-1) = (0, 1, 0)... expressed as -r x omega below.
        assertVectorEquals(test, "xlt shifts the linear reference point",
                           (X_t * spin).getLinearVelocity(), -r.cross(Vector3(1, 0, 0)));
    }

    /*
    ==========================
        Spatial cross products
    ==========================
    */

    {
        const SpatialMotion m(Vector3(-0.4, 0.9, 1.6), Vector3(0.7, -0.3, 2.2));
        assertVectorEquals(test, "motion cross product matches crm (eq. 2.31/2.33)",
                           v.cross(m).getVector(), crossMotionMatrix(v) * m.getVector());
        // crf = -crm^T (eq. 2.32)
        assertVectorEquals(test, "force cross product matches crf = -crm^T (eq. 2.32/2.34)",
                           v.cross(f).getVector(), (-crossMotionMatrix(v).getTranspose()) * f.getVector());
    }

    /*
    ===================
        Spatial inertia
    ===================
    */

    {
        const InertiaMatrix I_cm(Vector3(0.8, 0.1, -0.2), Vector3(0.1, 1.3, 0.05), Vector3(-0.2, 0.05, 1.7));
        const SpatialInertia inertia(2.5, Vector3(0.1, -0.4, 0.9), I_cm);

        assertVectorEquals(test, "spatial inertia applied to a velocity matches its 6x6 matrix (eq. 2.63)",
                           (inertia * v).getVector(), inertia.getMatrix() * v.getVector());

        assertMatrixEquals(test, "spatial inertia matrix is symmetric",
                           inertia.getMatrix(), inertia.getMatrix().getTranspose());

        // transformSpatialInertia follows the same direction convention as operator*: with X1 read as
        // B_X_A, it maps a quantity expressed in A into B.
        const SpatialInertia I_B = X1.transformSpatialInertia(inertia);

        // Kinetic energy is frame-invariant: 0.5 v.I v must not change when both the velocity and
        // the inertia are re-expressed in another frame.
        const double ke_A = 0.5 * v.getVector().dot(inertia.getMatrix() * v.getVector());
        const SpatialMotion v_B = X1 * v;
        const double ke_B = 0.5 * (v_B * (I_B * v_B));
        test.assertEquals("kinetic energy is invariant under transformSpatialInertia", ke_A, ke_B, 1e-10);

        assertMatrixEquals(test, "transformSpatialInertia equals X^-T I X^-1 (eq. 2.66-2.67)",
                           I_B.getMatrix(),
                           X1.getInverse().getMotionMatrix().getTranspose() * inertia.getMatrix()
                               * X1.getInverse().getMotionMatrix());

        assertMatrixEquals(test, "transformSpatialInertia round-trips through the inverse transform",
                           X1.getInverse().transformSpatialInertia(I_B).getMatrix(), inertia.getMatrix());
    }

    return test.getNumFails();
}
