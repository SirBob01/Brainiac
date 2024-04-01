#include <iostream>

#include "../../src/Board.hpp"

#include "ctest.hpp"

using namespace Brainiac;

int tests_run = 0;

static char *test_board_set() {
    Board board;
    Square sq = Square::D4;
    Piece piece = Piece::WhiteKing;

    board.set(sq, piece);

    Bitboard mask = get_square_mask(sq);
    mu_assert("Piece bitboard set", board.bitboard(piece) & mask);
    mu_assert("Color bitboard set", board.bitboard(Color::White) & mask);
    mu_assert("Color bitboard clear", !(board.bitboard(Color::Black) & mask));
    mu_assert("Piece list set", board.get(sq) == piece);
    return 0;
}

static char *test_board_set_overwrite() {
    Board board;
    Square sq = Square::D4;
    Piece piece0 = Piece::BlackRook;
    Piece piece1 = Piece::WhiteKing;

    board.set(sq, piece0);
    board.set(sq, piece1);

    Bitboard mask = get_square_mask(sq);
    mu_assert("Piece bitboard set", board.bitboard(piece1) & mask);
    mu_assert("Color bitboard set", board.bitboard(Color::White) & mask);
    mu_assert("Color bitboard clear", !(board.bitboard(Color::Black) & mask));
    mu_assert("Piece list set", board.get(sq) == piece1);
    return 0;
}

static char *test_board_get() {
    Board board;
    Square sq0 = Square::D4;
    Square sq1 = Square::D6;
    Piece piece = Piece::WhiteKing;

    board.set(sq0, piece);

    mu_assert("Board get", board.get(sq0) == piece);
    mu_assert("Board get (empty)", board.get(sq1) == Piece::Empty);
    return 0;
}

static char *test_board_copy() {
    Board board;
    Square sq = Square::D4;
    Piece piece = Piece::WhiteKing;

    board.set(sq, piece);

    Board copy = board;
    Bitboard mask = get_square_mask(sq);
    mu_assert("Piece bitboard set", copy.bitboard(piece) & mask);
    mu_assert("Color bitboard set", copy.bitboard(Color::White) & mask);
    mu_assert("Color bitboard clear", !(copy.bitboard(Color::Black) & mask));
    mu_assert("Board copy set", copy.get(sq) == piece);
    return 0;
}

static char *test_board_clear() {
    Board board;
    Square sq = Square::D4;
    Piece piece = Piece::WhiteKing;

    board.set(sq, piece);
    board.clear(sq);

    Bitboard mask = get_square_mask(sq);
    for (uint8_t i = 0; i < 12; i++) {
        Piece piece = static_cast<Piece>(i);
        mu_assert("Piece bitboard clear", !(board.bitboard(piece) & mask));
    }
    mu_assert("White bitboard clear", !(board.bitboard(Color::White) & mask));
    mu_assert("Black bitboard clear", !(board.bitboard(Color::Black) & mask));
    mu_assert("Piece list clear", board.get(sq) == Piece::Empty);
    return 0;
}

static char *test_board_clear_empty() {
    Board board;
    Square sq = Square::D4;

    board.clear(sq);

    Bitboard mask = get_square_mask(sq);
    for (uint8_t i = 0; i < 12; i++) {
        Piece piece = static_cast<Piece>(i);
        mu_assert("Piece bitboard clear", !(board.bitboard(piece) & mask));
    }
    mu_assert("White bitboard clear", !(board.bitboard(Color::White) & mask));
    mu_assert("Black bitboard clear", !(board.bitboard(Color::Black) & mask));
    mu_assert("Piece list clear", board.get(sq) == Piece::Empty);
    return 0;
}

static char *all_tests() {
    mu_run_test(test_board_set);
    mu_run_test(test_board_set_overwrite);
    mu_run_test(test_board_get);
    mu_run_test(test_board_clear);
    mu_run_test(test_board_clear_empty);
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