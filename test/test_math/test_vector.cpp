
#include "TestHarness.h"
#include "Vector.h"
#include <cstddef>


using namespace Stellarium;

int main() {
    
    Test test("Vector");

    // verify that we cannot copy-assign a vector of different length to an existing vector
    Vector vec1 { 1.0, 2.0, 3.0 };
    Vector vec2 { 1.0, 2.0 };

    bool result_copy_assign_with_bad_dimensions = false;

    try {
        vec1 = vec2;
    } catch (const std::invalid_argument&) {
        result_copy_assign_with_bad_dimensions = true;
    }

    test.assertTrue("Copy assign with bad dimensions", result_copy_assign_with_bad_dimensions);
    
    return test.getNumFails();
}