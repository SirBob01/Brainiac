#include <iostream>

#include "../../src/Bitboard.hpp"

#include "ctest.hpp"

using namespace Brainiac;

int tests_run = 0;

static char *test_count_set_bitboard() {
    Bitboard b1 = 1;
    mu_assert("Count set bits (1)", count_set_bitboard(b1) == 1);

    Bitboard b7 = 0b1100111010010;
    mu_assert("Count set bits (7)", count_set_bitboard(b7) == 7);

    Bitboard b0 = 0;
    mu_assert("Count set bits (0)", count_set_bitboard(b0) == 0);
    return 0;
}

static char *test_pop_lsb_bitboard() {
    Bitboard b1 = 1;
    mu_assert("Pop LSB (1)", pop_lsb_bitboard(b1) == 0);

    Bitboard b7 = 0b1100111010010;
    mu_assert("Pop LSB (7)", pop_lsb_bitboard(b7) == 0b1100111010000);

    Bitboard b0 = 0;
    mu_assert("Pop LSB (0)", pop_lsb_bitboard(b0) == 0);
    return 0;
}

static char *all_tests() {
    mu_run_test(test_count_set_bitboard);
    mu_run_test(test_pop_lsb_bitboard);
    return 0;
}

int main(int argc, char **argv) {
    char *result = all_tests();
    if (result != 0) {
        std::cout << "FAILED... " << result << "\n";
    } else {
        std::cout << "ALL TESTS PASSED\n";
    }
    std::cout << "Number of tests run: " << tests_run << "\n";

    return result != 0;
}