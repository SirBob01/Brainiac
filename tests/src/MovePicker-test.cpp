#include <iostream>

#include "../../src/Brainiac.hpp"

#include "ctest.hpp"

using namespace Brainiac;

int tests_run = 0;

static char *test_move_picker() {
    // Position position("8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1");
    Position position("r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1");
    History history;
    Node node;

    MovePicker picker;
    for (Move move : position.moves()) {
        picker.add(move, position, history, node);
    }

    MoveEntry entry;
    MoveValue value = MIN_MOVE_VALUE;
    MovePhase phase = MovePhase::HashPhase;
    unsigned total = 0;
    while (!picker.end()) {
        entry = picker.next();

        mu_assert("Move phase order preserved", entry.phase >= phase);
        if (entry.phase == phase) {
            mu_assert("Move value order preserved", entry.value >= value);
        }
        mu_assert("Move valid",
                  std::find(position.moves().begin(),
                            position.moves().end(),
                            entry.move) != position.moves().end());
        phase = entry.phase;
        total++;
    }

    mu_assert("Total moves visited", total == position.moves().size());
    return 0;
}

static char *all_tests() {
    mu_run_test(test_move_picker);
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