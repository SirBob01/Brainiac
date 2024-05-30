#include <iostream>
#include <string>

#include "../../src/Brainiac.hpp"

#include "ctest.hpp"

using namespace Brainiac;

int tests_run = 0;

static char *test_find_move() {
    Position pos("2K2r2/4P3/8/8/8/8/8/3k4 w - - 0 1");
    Move quiet = pos.find_move("c8c7");

    Move promote_queen = pos.find_move("e7e8q");
    Move promote_knight = pos.find_move("e7e8n");
    Move promote_bishop = pos.find_move("e7e8b");
    Move promote_rook = pos.find_move("e7e8r");

    Move capture_promote_queen = pos.find_move("e7f8q");
    Move capture_promote_knight = pos.find_move("e7f8n");
    Move capture_promote_bishop = pos.find_move("e7f8b");
    Move capture_promote_rook = pos.find_move("e7f8r");

    mu_assert("Quiet", quiet.type() == MoveType::Quiet);

    mu_assert("QueenPromote", promote_queen.type() == MoveType::QueenPromo);
    mu_assert("KnightPromote", promote_knight.type() == MoveType::KnightPromo);
    mu_assert("BishopPromote", promote_bishop.type() == MoveType::BishopPromo);
    mu_assert("RookPromote", promote_rook.type() == MoveType::RookPromo);

    mu_assert("QueenPromoteCapture",
              capture_promote_queen.type() == MoveType::QueenPromoCapture);
    mu_assert("KnightPromoteCapture",
              capture_promote_knight.type() == MoveType::KnightPromoCapture);
    mu_assert("BishopPromoteCapture",
              capture_promote_bishop.type() == MoveType::BishopPromoCapture);
    mu_assert("RookPromoteCapture",
              capture_promote_rook.type() == MoveType::RookPromoCapture);
    return 0;
}

static char *all_tests() {
    mu_run_test(test_find_move);
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