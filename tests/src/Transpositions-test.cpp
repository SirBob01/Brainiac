#include <iostream>

#include "../../src/Engine.hpp"

#include "ctest.hpp"

using namespace Brainiac;

int tests_run = 0;

static char *test_transpositions_initial() {
    Transpositions table;

    Position position;
    Node empty = table.get(position);
    mu_assert("Empty Node Type", empty.type == NodeType::Invalid);

    return 0;
}

static char *test_transpositions_set() {
    Transpositions table;

    Position position;
    NodeType type = NodeType::Exact;
    Depth depth = 3;
    Value value = 32;
    Move move(Square::E2, Square::E4, MoveType::Quiet);

    table.set(position, type, depth, value, move);

    Node node = table.get(position);
    mu_assert("Transposition Node Hash", node.hash == position.hash());
    mu_assert("Transposition Node Type", node.type == type);
    mu_assert("Transposition Node Depth", node.depth == depth);
    mu_assert("Transposition Node Value", node.value == value);
    mu_assert("Transposition Node Move", node.move == move);

    return 0;
}

static char *test_transpositions_overwrite() {
    Transpositions table;

    Position position;

    NodeType type = NodeType::Exact;
    Depth depth = 3;
    Value value = 32;
    Move move(Square::E2, Square::E4, MoveType::Quiet);

    table.set(position, type, depth, value, move);

    // Overwrite depth same

    NodeType new_type = NodeType::Lower;
    Depth new_depth = 3;
    Value new_value = 16;
    Move new_move(Square::D2, Square::D4, MoveType::Quiet);

    table.set(position, new_type, new_depth, new_value, new_move);

    Node node = table.get(position);
    mu_assert("Overtwrite Node Hash", node.hash == position.hash());
    mu_assert("Overtwrite Node Type", node.type == new_type);
    mu_assert("Overtwrite Node Depth", node.depth == new_depth);
    mu_assert("Overtwrite Node Value", node.value == new_value);
    mu_assert("Overtwrite Node Move", node.move == new_move);

    // Overwrite (subtree deep)

    new_type = NodeType::Exact;
    new_depth = 4;
    new_value = 16;
    new_move = Move(Square::E2, Square::D4, MoveType::Capture);

    table.set(position, new_type, new_depth, new_value, new_move);

    node = table.get(position);
    mu_assert("Overtwrite Node Hash", node.hash == position.hash());
    mu_assert("Overtwrite Node Type", node.type == new_type);
    mu_assert("Overtwrite Node Depth", node.depth == new_depth);
    mu_assert("Overtwrite Node Value", node.value == new_value);
    mu_assert("Overtwrite Node Move", node.move == new_move);

    // Do not overwrite (subtree shallow)

    NodeType skip_type = NodeType::Upper;
    Depth skip_depth = 3;
    Value skip_value = 12;
    Move skip_move(Square::A2, Square::B4, MoveType::Quiet);

    table.set(position, skip_type, skip_depth, skip_value, skip_move);

    node = table.get(position);
    mu_assert("Overtwrite Node Hash", node.hash == position.hash());
    mu_assert("Overtwrite Node Type", node.type == new_type);
    mu_assert("Overtwrite Node Depth", node.depth == new_depth);
    mu_assert("Overtwrite Node Value", node.value == new_value);
    mu_assert("Overtwrite Node Move", node.move == new_move);

    return 0;
}

static char *test_transpositions_clear() {
    Transpositions table;

    Position position;
    NodeType type = NodeType::Exact;
    Depth depth = 3;
    Value value = 32;
    Move move(Square::E2, Square::E4, MoveType::Quiet);

    table.set(position, type, depth, value, move);
    table.clear();

    Node empty = table.get(position);
    mu_assert("Empty Node Type", empty.type == NodeType::Invalid);

    return 0;
}

static char *all_tests() {
    mu_run_test(test_transpositions_initial);
    mu_run_test(test_transpositions_set);
    mu_run_test(test_transpositions_overwrite);
    mu_run_test(test_transpositions_clear);
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