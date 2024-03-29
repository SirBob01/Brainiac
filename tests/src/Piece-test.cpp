#include <iostream>

#include "../../src/Piece.hpp"

#include "ctest.hpp"

using namespace Brainiac;

int tests_run = 0;

static char *test_piece_create() {
    Piece piece(PieceType::Pawn, Color::Black);
    mu_assert("Piece Type", piece.type() == PieceType::Pawn);
    mu_assert("Piece Color", piece.color() == Color::Black);
    mu_assert("Piece index", piece.index() == 7);
    mu_assert("Piece character", piece.character() == 'p');
    mu_assert("Piece character", piece.icon() == "\u265F");
    return 0;
}

static char *test_piece_empty() {
    Piece piece;
    mu_assert("Piece Color", piece.type() == PieceType::Empty);
    return 0;
}

static char *all_tests() {
    mu_run_test(test_piece_create);
    mu_run_test(test_piece_empty);
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