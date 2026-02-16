
#include "TestHarness.h"
#include "SquareMatrix.h"
#include "Vector.h"
#include <cstddef>
#include <stdexcept>
#include <vector>

using namespace Stellarium;

int main() {
    
    Test test("Matrix");
    
    /* 
    =======================
            matrix 1
    =======================
    */
    Matrix mat1 {{ 
        {1.0, 2.0, 3.0, 4.0}, 
        {5.0, 6.0, 7.0, 8.0} 
    }};

    Matrix mat1_t { { 
        {1.0, 5.0}, 
        {2.0, 6.0}, 
        {3.0, 7.0}, 
        {4.0, 8.0}
    } };

    Matrix mat2 { { 
        {1.0, 2.0, -3.0 }, 
        {4.0, 5.0, 6.0}, 
        { -7.0, 8.0, 9.0 }, 
        { 10.0, 11.0, 12.0 } 
    }};

    Matrix mat2_t { { 
        {1.0, 4.0, -7.0 , 10.0}, 
        {2.0, 5.0, 8.0, 11.0}, 
        {-3.0, 6.0, 9.0, 12.0 }
    }};

    // test constant multiplication
    Matrix mat1_times_c {{
        {-2.0, -4.0, -6.0, -8.0}, 
        {-10.0, -12.0, -14.0, -16.0} 
    }};

    test.assertTrue("Matrix times constant 1", mat1 * -2.0 == mat1_times_c);
    test.assertTrue("Matrix times constant 2", -2.0 * mat1 == mat1_times_c);

    // test matrix-vector multiplication
    Vector vec1 {{-1.0, 2.0, -3.0, -4.0}};

    Vector mat1_times_vec1 {{-22, -46}};

    test.assertTrue("Matrix-vector multiplication 1", mat1 * vec1 == mat1_times_vec1);

    // test matrix transpose
    test.assertTrue("Matrix transpose 1", mat1.getTranspose() == mat1_t);
    test.assertTrue("Matrix transpose 2", mat2.getTranspose() == mat2_t);

    // test matrix multiplication
    Matrix mat_1_times_2 {{ 
        {28, 80, 84},
        {60, 184, 180}
    }};

    test.assertTrue("Matrix multiplication", mat1*mat2 == mat_1_times_2);

    // test column-major array
    std::vector<float> array1 {1.0f, 5.0f, 2.0f, 6.0f, 3.0f, 7.0f, 4.0f, 8.0f};

    test.assertTrue("Col-major array", mat1.getColMajorArray() == array1);

    // test that we cannot assign a new row to a Matrix that has incompatible shape
    bool result_assign_bad_row = false;

    try {
        mat1[1] = { 55.0, 67.0 };
    } catch (const std::invalid_argument&) {
        result_assign_bad_row = true;
    }

    test.assertTrue("Set bad row caught by Vector", result_assign_bad_row);

    // test that we cannot create a mstrix with incompatible dimensions
    bool result_create_matrix_with_bad_dimensions = false;

    try {
        Matrix mat3 { { {1.0, 2.0 }, {3.0, 4.0, 5.0} } };
    } catch (const std::invalid_argument&) {
        result_create_matrix_with_bad_dimensions = true;
    }

    test.assertTrue("Create Matrix with bad dimensions", result_create_matrix_with_bad_dimensions);
    
    // test that we cannot create a SquareMatrix from non-square data
        // test that we cannot create a mstrix with incompatible dimensions
    bool result_create_square_matrix_with_bad_dimensions = false;

    try {
        SquareMatrix mat4 { { {1.0, 2.0 }, {3.0, 4.0, }, {5.0, 6.0} } };
    } catch (const std::invalid_argument&) {
        result_create_square_matrix_with_bad_dimensions = true;
    }

    test.assertTrue("Create SquareMatrix with bad dimensions", result_create_square_matrix_with_bad_dimensions);

    return test.getNumFails();
}