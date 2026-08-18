
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

    // ...but a default-constructed ("unsized") vector adopts the size of whatever is assigned to
    // it. This is what lets fixed-size Vector members be filled in after construction, e.g.
    // Joint::Info::q_init.
    Vector unsized;
    test.assertTrue("Default-constructed vector is empty", unsized.getSize() == 0);

    unsized = Vector { 4.0, 5.0 };
    test.assertTrue("Unsized vector adopts assigned size", unsized.getSize() == 2);
    test.assertTrue("Unsized vector adopts assigned values", unsized == Vector({ 4.0, 5.0 }));

    // Once it has a size, it is fixed again.
    bool result_reassign_after_adopting = false;
    try {
        unsized = vec1;
    } catch (const std::invalid_argument&) {
        result_reassign_after_adopting = true;
    }
    test.assertTrue("Adopted size is then fixed", result_reassign_after_adopting);

    // Same-size assignment still works.
    unsized = Vector { 6.0, 7.0 };
    test.assertTrue("Same-size assignment still works", unsized == Vector({ 6.0, 7.0 }));

    return test.getNumFails();
}