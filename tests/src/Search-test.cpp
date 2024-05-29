#include <iostream>
#include <string>

#include "../../src/Brainiac.hpp"

#include "ctest.hpp"

using namespace Brainiac;

int tests_run = 0;

struct MateTestCase {
    std::string fen;
    std::vector<Move> sequence;
};

static std::string fen_label = "";
static std::string move_label = "";

std::vector<MateTestCase> POSITIONS = {
    {"r5rk/5p1p/5R2/4B3/8/8/7P/7K w - - 0 0",
     {
         Move(Square::F6, Square::A6, MoveType::Quiet),
         Move(Square::F7, Square::F6, MoveType::Quiet),
         Move(Square::E5, Square::F6, MoveType::Capture),
         Move(Square::G8, Square::G7, MoveType::Quiet),
         Move(Square::A6, Square::A8, MoveType::Capture),
     }},
};

static char *test_mate_in_n() {
    for (MateTestCase &test : POSITIONS) {
        Position position(test.fen);
        Search search;

        Color bot_turn = position.turn();

        for (unsigned i = 0; i < test.sequence.size(); i++) {
            if (position.turn() == bot_turn) {
                Result result = search.search(position);

                move_label = "Searched move (" +
                             test.sequence[i].standard_notation() +
                             ") = " + result.move.standard_notation();
                mu_assert(move_label.c_str(), result.move == test.sequence[i]);
            }
            position.make(test.sequence[i]);
        }

        fen_label = "Checkmate (" + test.fen + ")";
        mu_assert(fen_label.c_str(), position.is_checkmate());
    }
    return 0;
}

static char *all_tests() {
    mu_run_test(test_mate_in_n);
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