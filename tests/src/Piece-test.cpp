#include <iostream>

#include "../../src/Piece.hpp"

#include "ctest.hpp"

using namespace Brainiac;

int tests_run = 0;

static char *test_piece_index() {
    Piece piece = Piece::BlackPawn;
    mu_assert("Piece index", static_cast<uint8_t>(piece) == 7);
    mu_assert("Piece character",
              PIECE_CHARS[static_cast<uint8_t>(piece)] == 'p');
    mu_assert("Piece icon",
              PIECE_ICONS[static_cast<uint8_t>(piece)] == "\u265F");
    return 0;
}

static char *all_tests() {
    mu_run_test(test_piece_index);
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