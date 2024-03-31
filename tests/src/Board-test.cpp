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
    mu_assert("Piece bitboard set", board.bitboard(white_king) & mask);
    mu_assert("Color bitboard set", board.bitboard(Color::White) & mask);
    mu_assert("Piece list set", board.get(sq) == white_king);
    return 0;
}

static char *test_board_set_overwrite() {
    Board board;
    Square sq = Square::D4;
    Piece black_rook(PieceType::Rook, Color::Black);
    Piece white_king(PieceType::King, Color::White);

    board.set(sq, black_rook);
    board.set(sq, white_king);

    Bitboard mask = get_square_mask(sq);
    mu_assert("Piece bitboard set", board.bitboard(white_king) & mask);
    mu_assert("Color bitboard set", board.bitboard(Color::White) & mask);
    mu_assert("Piece list set", board.get(sq) == white_king);
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

static char *test_board_copy() {
    Board board;
    Square sq = Square::D4;
    Piece white_king(PieceType::King, Color::White);

    board.set(sq, white_king);

    Board copy = board;
    Bitboard mask = get_square_mask(sq);
    mu_assert("Piece bitboard set", copy.bitboard(white_king) & mask);
    mu_assert("Color bitboard set", copy.bitboard(Color::White) & mask);
    mu_assert("Board copy set", copy.get(sq) == white_king);
    return 0;
}

static char *test_board_clear() {
    Board board;
    Square sq = Square::D4;
    Piece white_king(PieceType::King, Color::White);

    board.set(sq, white_king);
    board.clear(sq);

    Bitboard mask = get_square_mask(sq);
    mu_assert("Piece bitboard clear", !(board.bitboard(white_king) & mask));
    mu_assert("Color bitboard clear", !(board.bitboard(Color::White) & mask));
    mu_assert("Piece list clear", board.get(sq).empty());
    return 0;
}

static char *all_tests() {
    mu_run_test(test_board_set);
    mu_run_test(test_board_set_overwrite);
    mu_run_test(test_board_get);
    mu_run_test(test_board_clear);
    mu_run_test(test_board_copy);
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