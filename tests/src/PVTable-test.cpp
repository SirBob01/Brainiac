#include <iostream>

#include "../../src/Engine.hpp"

#include "ctest.hpp"

using namespace Brainiac;

int tests_run = 0;

static char *test_pv_update() {
    PVTable table;
    Move a(Square::E2, Square::E4, MoveType::Quiet);
    Move b(Square::D7, Square::D5, MoveType::Quiet);

    table.update(1, b);
    table.update(0, a);

    mu_assert("Length at depth 1", table.get_length(1) == 1);
    mu_assert("Length at depth 0", table.get_length(0) == 2);

    mu_assert("PV[0]", table.get(0, 0) == a);
    mu_assert("PV[1]", table.get(0, 1) == b);

    return 0;
}

static char *test_pv_clear() {
    PVTable table;
    Move a(Square::E2, Square::E4, MoveType::Quiet);
    Move b(Square::D7, Square::D5, MoveType::Quiet);

    table.update(1, b);
    table.update(0, a);

    mu_assert("Length at depth 1", table.get_length(1) == 1);
    mu_assert("Length at depth 0", table.get_length(0) == 2);

    table.clear(0);

    mu_assert("Length at depth 1", table.get_length(1) == 1);
    mu_assert("Length at depth 0", table.get_length(0) == 0);

    table.clear(1);

    mu_assert("Length at depth 1", table.get_length(1) == 0);
    mu_assert("Length at depth 0", table.get_length(0) == 0);

    return 0;
}

static char *all_tests() {
    mu_run_test(test_pv_update);
    mu_run_test(test_pv_clear);
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