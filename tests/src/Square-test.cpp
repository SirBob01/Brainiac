#include <iostream>

#include "../../src/Engine.hpp"

#include "ctest.hpp"

using namespace Brainiac;

int tests_run = 0;

static char *test_square_to_string() {
    mu_assert("A1 Square String", square_to_string(Square::A1) == "a1");
    mu_assert("E5 Square String", square_to_string(Square::E5) == "e5");
    mu_assert("H8 Square String", square_to_string(Square::H8) == "h8");
    return 0;
}

static char *test_string_to_square() {
    mu_assert("A1 String Square", string_to_square("a1") == Square::A1);
    mu_assert("E5 String Square", string_to_square("e5") == Square::E5);
    mu_assert("H8 String Square", string_to_square("h8") == Square::H8);
    return 0;
}

static char *all_tests() {
    mu_run_test(test_square_to_string);
    mu_run_test(test_string_to_square);
    return 0;
}

int main(int argc, char **argv) {
    init();
    char *result = all_tests();
    if (result != 0) {
        std::cout << "FAILED... " << result << "\n";
    } else {
        std::cout << "ALL TESTS PASSED\n";
    }
    std::cout << "Number of tests run: " << tests_run << "\n";

    return result != 0;
}