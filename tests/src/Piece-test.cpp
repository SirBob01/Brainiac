#include <iostream>

#include "../../src/Brainiac.hpp"

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

static char *test_create_piece() {
    mu_assert("Create BlackKnight",
              create_piece(PieceType::Knight, Color::Black) ==
                  Piece::BlackKnight);
    mu_assert("Create WhiteRook",
              create_piece(PieceType::Rook, Color::White) == Piece::WhiteRook);
    return 0;
}

static char *test_get_piece_color() {
    mu_assert("WhiteQueen", get_piece_color(Piece::WhiteQueen) == Color::White);
    mu_assert("WhiteBishop",
              get_piece_color(Piece::WhiteBishop) == Color::White);
    mu_assert("WhiteRook", get_piece_color(Piece::WhiteRook) == Color::White);
    mu_assert("WhitePawn", get_piece_color(Piece::WhitePawn) == Color::White);
    mu_assert("WhiteKing", get_piece_color(Piece::WhiteKing) == Color::White);
    mu_assert("WhiteKnight",
              get_piece_color(Piece::WhiteKnight) == Color::White);

    mu_assert("BlackQueen", get_piece_color(Piece::BlackQueen) == Color::Black);
    mu_assert("BlackBishop",
              get_piece_color(Piece::BlackBishop) == Color::Black);
    mu_assert("BlackRook", get_piece_color(Piece::BlackRook) == Color::Black);
    mu_assert("BlackPawn", get_piece_color(Piece::BlackPawn) == Color::Black);
    mu_assert("BlackKing", get_piece_color(Piece::BlackKing) == Color::Black);
    mu_assert("BlackKnight",
              get_piece_color(Piece::BlackKnight) == Color::Black);
    return 0;
}

static char *all_tests() {
    mu_run_test(test_piece_index);
    mu_run_test(test_create_piece);
    mu_run_test(test_get_piece_color);
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