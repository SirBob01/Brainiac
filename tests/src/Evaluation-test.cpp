#include <iostream>

#include "../../src/Evaluation.hpp"

#include "ctest.hpp"

using namespace Brainiac;

int tests_run = 0;

static std::string fen_label = "";
static std::string test_label = "";

static char *test_compute_material() {
    Position pos;
    mu_assert("Start position", compute_material(pos.board()) == 0);

    std::vector<std::string> fens = {
        DEFAULT_BOARD_FEN,
        "8/3BRr1p/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1",
    };
    for (const std::string &fen : fens) {
        Position pos(fen);

        int value = compute_material(pos.board());
        int expected = 0;

        // Compute it from the FEN string
        std::string layout = tokenize(fen)[0];
        std::cout << layout << "\n";
        for (char c : layout) {
            switch (c) {
            case 'p':
                expected += PIECE_WEIGHTS[Piece::BlackPawn];
                break;
            case 'n':
                expected += PIECE_WEIGHTS[Piece::BlackKnight];
                break;
            case 'r':
                expected += PIECE_WEIGHTS[Piece::BlackRook];
                break;
            case 'b':
                expected += PIECE_WEIGHTS[Piece::BlackBishop];
                break;
            case 'q':
                expected += PIECE_WEIGHTS[Piece::BlackQueen];
                break;
            case 'k':
                expected += PIECE_WEIGHTS[Piece::BlackKing];
                break;

            case 'P':
                expected += PIECE_WEIGHTS[Piece::WhitePawn];
                break;
            case 'N':
                expected += PIECE_WEIGHTS[Piece::WhiteKnight];
                break;
            case 'R':
                expected += PIECE_WEIGHTS[Piece::WhiteRook];
                break;
            case 'B':
                expected += PIECE_WEIGHTS[Piece::WhiteBishop];
                break;
            case 'Q':
                expected += PIECE_WEIGHTS[Piece::WhiteQueen];
                break;
            case 'K':
                expected += PIECE_WEIGHTS[Piece::WhiteKing];
                break;
            }
        }

        fen_label = "Position FEN (" + fen + ")";
        test_label = "Position material (" + fen + ")";
        mu_assert(fen_label.c_str(), pos.fen() == fen);
        mu_assert(test_label.c_str(), value == expected);
    }

    return 0;
}

static char *test_compute_placement() {
    Position pos;
    mu_assert("Start position", compute_placement(pos.board()) == 0);
    return 0;
}

static char *test_evaluate() {
    Position pos;
    mu_assert("Start position", evaluate(pos) == 0);
    return 0;
}

static char *all_tests() {
    mu_run_test(test_compute_material);
    mu_run_test(test_compute_placement);
    mu_run_test(test_evaluate);
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