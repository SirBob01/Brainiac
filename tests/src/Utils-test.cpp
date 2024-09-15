#include <iostream>

#include "../../src/Engine.hpp"

#include "ctest.hpp"

using namespace Brainiac;

int tests_run = 0;

static char *test_tokenize() {
    std::vector<std::string> tokens;
    tokens = tokenize("hello, world");
    mu_assert("'hello, world' tokens",
              tokens[0] == "hello," && tokens[1] == "world");

    tokens = tokenize("single");
    mu_assert("'single' tokens", tokens[0] == "single");

    tokens = tokenize("  random  arbitrary   spacing ");
    mu_assert("'  random  arbitrary   spacing ' tokens",
              tokens[0] == "random" && tokens[1] == "arbitrary" &&
                  tokens[2] == "spacing");

    tokens = tokenize("    ");
    mu_assert("'    ' tokens (empty)", tokens.empty());
    return 0;
}

static char *all_tests() {
    mu_run_test(test_tokenize);
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