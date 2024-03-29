#include <iostream>

#include "../../src/Board.hpp"

#include "ctest.hpp"

using namespace Brainiac;

int tests_run = 0;

static char *test_board_set() {
    Board board;
    Square sq = Square::D4;
    Piece white_king(PieceType::King, Color::White);

    board.set(sq, white_king);

    Bitboard mask = get_square_mask(sq);
    mu_assert("Piece bitboard set",
              board._bitboards[white_king.index()] & mask);
    mu_assert("Color bitboard set",
              board._bitboards[12 + static_cast<uint8_t>(Color::White)] & mask);
    mu_assert("Piece list set", board._pieces[sq] == white_king);
    return 0;
}

static char *test_board_get() {
    Board board;
    Square sq = Square::D4;
    Piece white_king(PieceType::King, Color::White);

    board.set(sq, white_king);

    mu_assert("Board get", board.get(sq) == white_king);
    return 0;
}

static char *all_tests() {
    mu_run_test(test_board_set);
    mu_run_test(test_board_get);
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