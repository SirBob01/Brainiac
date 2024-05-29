#include <iostream>

#include "../../src/Brainiac.hpp"

#include "ctest.hpp"

using namespace Brainiac;

int tests_run = 0;

static char *test_find_lsb_bitboard() {
    Bitboard b0 = 1;
    mu_assert("Find lsb bits 0", find_lsb_bitboard(b0) == 0);

    Bitboard b1 = 0b1100111010000;
    mu_assert("Find lsb bits 1", find_lsb_bitboard(b1) == 4);

    return 0;
}

static char *test_count_set_bitboard() {
    Bitboard b0 = 1;
    mu_assert("Count set bits 0", count_set_bitboard(b0) == 1);

    Bitboard b1 = 0b1100111010010;
    mu_assert("Count set bits 1", count_set_bitboard(b1) == 7);

    Bitboard b2 = 0;
    mu_assert("Count set bits 2", count_set_bitboard(b2) == 0);
    return 0;
}

static char *test_flip_vertical_bitboard() {
    Bitboard b0 = 1;
    mu_assert("Flip vertical 0",
              flip_vertical_bitboard(b0) ==
                  0b100000000000000000000000000000000000000000000000000000000);

    Bitboard b1 = 0b1100111010000;
    mu_assert(
        "Flip vertical 1",
        flip_vertical_bitboard(b1) ==
            0b1101000000011001000000000000000000000000000000000000000000000000);
    return 0;
}

static char *test_flip_horizontal_bitboard() {
    Bitboard b0 = 1;
    mu_assert("Flip horizontal 0", flip_horizontal_bitboard(b0) == 0b10000000);

    Bitboard b1 = 0b1100111010000;
    mu_assert("Flip horizontal 1",
              flip_horizontal_bitboard(b1) == 0b1001100000001011);
    return 0;
}

static char *test_get_lsb_bitboard() {
    Bitboard b0 = 1;
    mu_assert("Get LSB 0", get_lsb_bitboard(b0) == 1);

    Bitboard b1 = 0b1100111010000;
    mu_assert("Get LSB 1", get_lsb_bitboard(b1) == 0b10000);

    return 0;
}

static char *test_pop_lsb_bitboard() {
    Bitboard b0 = 1;
    mu_assert("Pop LSB 0", pop_lsb_bitboard(b0) == 0);

    Bitboard b1 = 0b1100111010010;
    mu_assert("Pop LSB 1", pop_lsb_bitboard(b1) == 0b1100111010000);

    Bitboard b2 = 0;
    mu_assert("Pop LSB 2", pop_lsb_bitboard(b2) == 0);
    return 0;
}

static char *all_tests() {
    mu_run_test(test_find_lsb_bitboard);
    mu_run_test(test_count_set_bitboard);
    mu_run_test(test_flip_vertical_bitboard);
    mu_run_test(test_flip_horizontal_bitboard);
    mu_run_test(test_get_lsb_bitboard);
    mu_run_test(test_pop_lsb_bitboard);
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