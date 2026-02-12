
#include "TestHarness.h"
#include "Matrix.h"
#include "Vector.h"
#include <array>

using namespace Stellarium;

int main() {
    
    Test test("Matrix");
    
    /* 
    =======================
            matrix 1
    =======================
    */
    Matrix<2, 4> mat1 {{ 
        Vector<4>({1.0, 2.0, 3.0, 4.0}), 
        Vector<4>({5.0, 6.0, 7.0, 8.0}) 
    }};

    Matrix<4, 2> mat1_t { { 
        Vector<2>({1.0, 5.0}), 
        Vector<2>({2.0, 6.0}), 
        Vector<2>({3.0, 7.0}), 
        Vector<2>({4.0, 8.0})
    } };

    Matrix<4, 3> mat2 { { 
        Vector<3>({1.0, 2.0, -3.0 }), 
        Vector<3>({4.0, 5.0, 6.0}), 
        Vector<3>({ -7.0, 8.0, 9.0 }), 
        Vector<3>({ 10.0, 11.0, 12.0 }) 
    }};

    Matrix<3, 4> mat2_t { { 
        Vector<4>({1.0, 4.0, -7.0 , 10.0}), 
        Vector<4>({2.0, 5.0, 8.0, 11.0}), 
        Vector<4>({-3.0, 6.0, 9.0, 12.0 })
    }};

    // test constant multiplication
    Matrix<2, 4> mat1_times_c {{
        Vector<4>({-2.0, -4.0, -6.0, -8.0}), 
        Vector<4>({-10.0, -12.0, -14.0, -16.0}) 
    }};

    test.assertTrue("Matrix times constant 1", mat1 * -2.0 == mat1_times_c);
    test.assertTrue("Matrix times constant 2", -2.0 * mat1 == mat1_times_c);

    // test matrix-vector multiplication
    Vector<4> vec1 {{-1.0, 2.0, -3.0, -4.0}};

    Vector<2> mat1_times_vec1 {{-22, -46}};

    test.assertTrue("Matrix-vector multiplication 1", mat1 * vec1 == mat1_times_vec1);

    // test matrix transpose
    test.assertTrue("Matrix transpose 1", mat1.getTranspose() == mat1_t);
    test.assertTrue("Matrix transpose 2", mat2.getTranspose() == mat2_t);

    // test matrix multiplication
    Matrix<2, 3> mat_1_times_2 {{ 
        Vector<3>({28, 80, 84}),
        Vector<3>({60, 184, 180})
    }};

    test.assertTrue("Matrix multiplication", mat1*mat2 == mat_1_times_2);

    // test column-major array
    std::array<float, 8> array1 {1.0f, 5.0f, 2.0f, 6.0f, 3.0f, 7.0f, 4.0f, 8.0f};

    test.assertTrue("Col-major array", mat1.getColMajorArray() == array1);

    return test.getNumFails();
}