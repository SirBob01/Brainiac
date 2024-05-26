#include <iostream>

#include "../../src/History.hpp"

#include "ctest.hpp"

using namespace Brainiac;

int tests_run = 0;

static char *test_history() {
    History history;

    Move capture(Square::A1, Square::A2, MoveType::Capture);
    Move capture_promo(Square::B1, Square::B2, MoveType::QueenPromoCapture);

    Move quiet(Square::D1, Square::D2, MoveType::Quiet);
    Move promo(Square::C1, Square::C2, MoveType::QueenPromo);

    Depth depth0 = 4;
    Depth depth1 = 5;

    history.set(capture, depth0);
    history.set(capture, depth1);

    history.set(capture_promo, depth0);
    history.set(capture_promo, depth1);

    history.set(quiet, depth0);
    history.set(quiet, depth1);

    history.set(promo, depth0);
    history.set(promo, depth1);

    // Captures don't update history
    mu_assert("Capture history", history.get(capture) == 0);
    mu_assert("Capture promote history", history.get(capture_promo) == 0);

    // Non-captures
    int expected = (depth0 * depth0) + (depth1 * depth1);
    mu_assert("Quiet history", history.get(quiet) == expected);
    mu_assert("Promote history", history.get(promo) == expected);

    return 0;
}

static char *all_tests() {
    mu_run_test(test_history);
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