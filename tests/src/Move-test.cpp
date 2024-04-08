#include <iostream>

#include "../../src/Move.hpp"

#include "ctest.hpp"

using namespace Brainiac;

int tests_run = 0;

static char *test_move_quiet() {
    Move move(Square::A1, Square::H8, MoveType::Quiet);
    mu_assert("Move src - A1", move.src() == Square::A1);
    mu_assert("Move dst - H8", move.dst() == Square::H8);
    mu_assert("Move type - Quiet", move.type() == MoveType::Quiet);
    mu_assert("Move string", move.standard_notation() == "a1h8");
    return 0;
}

static char *test_move_promote() {
    {
        Move r(Square::A1, Square::H8, MoveType::RookPromo);
        Move r_capture(Square::A1, Square::H8, MoveType::RookPromoCapture);

        mu_assert("RookPromo type", r.type() == MoveType::RookPromo);
        mu_assert("RookPromoCapture type",
                  r_capture.type() == MoveType::RookPromoCapture);
        mu_assert("Rook 'r' suffix", r.standard_notation() == "a1h8r");
        mu_assert("Rook matching move string",
                  r.standard_notation() == r_capture.standard_notation());
    }

    {
        Move b(Square::A1, Square::H8, MoveType::BishopPromo);
        Move b_capture(Square::A1, Square::H8, MoveType::BishopPromoCapture);

        mu_assert("BishopPromo type", b.type() == MoveType::BishopPromo);
        mu_assert("BishopPromoCapture type",
                  b_capture.type() == MoveType::BishopPromoCapture);
        mu_assert("Bishop 'b' suffix", b.standard_notation() == "a1h8b");
        mu_assert("Bishop matching move string",
                  b.standard_notation() == b_capture.standard_notation());
    }

    {
        Move n(Square::A1, Square::H8, MoveType::KnightPromo);
        Move n_capture(Square::A1, Square::H8, MoveType::KnightPromoCapture);

        mu_assert("KnightPromo type", n.type() == MoveType::KnightPromo);
        mu_assert("KnightPromoCapture type",
                  n_capture.type() == MoveType::KnightPromoCapture);
        mu_assert("Knight 'n' suffix", n.standard_notation() == "a1h8n");
        mu_assert("Knight matching move string",
                  n.standard_notation() == n_capture.standard_notation());
    }

    {
        Move q(Square::A1, Square::H8, MoveType::QueenPromo);
        Move q_capture(Square::A1, Square::H8, MoveType::QueenPromoCapture);

        mu_assert("QueenPromo type", q.type() == MoveType::QueenPromo);
        mu_assert("QueenPromoCapture type",
                  q_capture.type() == MoveType::QueenPromoCapture);
        mu_assert("Queen 'q' suffix", q.standard_notation() == "a1h8q");
        mu_assert("Queen matching move string",
                  q.standard_notation() == q_capture.standard_notation());
    }

    return 0;
}

static char *test_move_equality() {
    Move a(Square::A1, Square::H8, MoveType::Quiet);
    Move b(Square::A1, Square::H8, MoveType::Quiet);

    Move c(Square::A1, Square::H8, MoveType::RookPromo);
    Move d(Square::A1, Square::H8, MoveType::RookPromoCapture);

    mu_assert("Move equality", a == b);
    mu_assert("Move inequality", c != d);
    return 0;
}

static char *all_tests() {
    mu_run_test(test_move_quiet);
    mu_run_test(test_move_promote);
    mu_run_test(test_move_equality);
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