#include <iostream>

#include "../../src/Engine.hpp"

#include "ctest.hpp"

using namespace Brainiac;

int tests_run = 0;

static char *test_move_list_add() {
    MoveList list;
    mu_assert("Empty list", list.size() == 0);
    list.add(Square::A1, Square::H8, MoveType::Capture);
    mu_assert("Non-empty list", list.size() == 1);
    return 0;
}

static char *test_move_list_clear() {
    MoveList list;
    list.add(Square::A1, Square::H8, MoveType::Capture);
    list.add(Square::B1, Square::G8, MoveType::EnPassant);
    mu_assert("Non-empty list", list.size() == 2);
    list.clear();
    mu_assert("Cleared list", list.size() == 0);
    return 0;
}

static char *test_move_list_access() {
    MoveList list;
    list.add(Square::A1, Square::H8, MoveType::Capture);
    list.add(Square::B1, Square::G8, MoveType::EnPassant);

    mu_assert("list[0].src()", list[0].src() == Square::A1);
    mu_assert("list[0].dst()", list[0].dst() == Square::H8);
    mu_assert("list[0].type()", list[0].type() == MoveType::Capture);

    mu_assert("list[1].src()", list[1].src() == Square::B1);
    mu_assert("list[1].dst()", list[1].dst() == Square::G8);
    mu_assert("list[1].type()", list[1].type() == MoveType::EnPassant);

    return 0;
}

static char *test_move_list_iterator() {
    MoveList list;
    list.add(Square::A1, Square::H8, MoveType::Capture);
    list.add(Square::C2, Square::D3, MoveType::Quiet);
    list.add(Square::B1, Square::G8, MoveType::EnPassant);

    mu_assert("List size", list.size() == 3);

    mu_assert("list.begin()->src()", list.begin()->src() == Square::A1);
    mu_assert("list.begin()->dst()", list.begin()->dst() == Square::H8);
    mu_assert("list.begin()->type()",
              list.begin()->type() == MoveType::Capture);

    mu_assert("list.(end() - 1)->src()", (list.end() - 1)->src() == Square::B1);
    mu_assert("list.(end() - 1)->dst()", (list.end() - 1)->dst() == Square::G8);
    mu_assert("list.(end() - 1)->type()",
              (list.end() - 1)->type() == MoveType::EnPassant);

    return 0;
}

static char *all_tests() {
    mu_run_test(test_move_list_add);
    mu_run_test(test_move_list_clear);
    mu_run_test(test_move_list_access);
    mu_run_test(test_move_list_iterator);
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