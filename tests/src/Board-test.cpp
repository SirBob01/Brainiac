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
    std::array<Bitboard, 14> bitboards = board.bitboards();
    std::array<Piece, 64> pieces = board.pieces();
    mu_assert("Piece bitboard set", bitboards[white_king.index()] & mask);
    mu_assert("Color bitboard set",
              bitboards[12 + static_cast<uint8_t>(Color::White)] & mask);
    mu_assert("Piece list set", pieces[sq] == white_king);
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

static char *test_board_clear() {
    Board board;
    Square sq = Square::D4;
    Piece white_king(PieceType::King, Color::White);

    board.set(sq, white_king);
    board.clear(sq);

    Bitboard mask = get_square_mask(sq);
    std::array<Bitboard, 14> bitboards = board.bitboards();
    std::array<Piece, 64> pieces = board.pieces();
    mu_assert("Piece bitboard clear", !(bitboards[white_king.index()] & mask));
    mu_assert("Color bitboard clear",
              !(bitboards[12 + static_cast<uint8_t>(Color::White)] & mask));
    mu_assert("Piece list clear", pieces[sq].type() == PieceType::Empty);
    return 0;
}

static char *all_tests() {
    mu_run_test(test_board_set);
    mu_run_test(test_board_get);
    mu_run_test(test_board_clear);
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