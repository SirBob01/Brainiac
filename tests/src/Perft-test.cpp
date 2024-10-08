#include <array>
#include <iostream>

#include "../../src/Engine.hpp"

#include "ctest.hpp"

using namespace Brainiac;

int tests_run = 0;

struct PerftTestCase {
    std::string fen;
    Depth depth;
    uint64_t result;
};

std::vector<PerftTestCase> POSITIONS = {
    //--Illegal ep move #1
    {"3k4/3p4/8/K1P4r/8/8/8/8 b - - 0 1", 6, 1134888},

    //--Illegal ep move #2
    {"8/8/4k3/8/2p5/8/B2P2K1/8 w - - 0 1", 6, 1015133},

    //--EP Capture Checks Opponent
    {"8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1", 6, 1440467},

    //--Short Castling Gives Check
    {"5k2/8/8/8/8/8/8/4K2R w K - 0 1", 6, 661072},

    //--Long Castling Gives Check
    {"3k4/8/8/8/8/8/8/R3K3 w Q - 0 1", 6, 803711},

    //--Castle Rights
    {"r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1", 4, 1274206},

    //--Castling Prevented
    {"r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1", 4, 1720476},

    //--Promote out of Check
    {"2K2r2/4P3/8/8/8/8/8/3k4 w - - 0 1", 6, 3821001},

    //--Discovered Check
    {"8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1", 5, 1004658},

    //--Promote to give check
    {"4k3/1P6/8/8/8/8/K7/8 w - - 0 1", 6, 217342},

    //--Under Promote to give check
    {"8/P1k5/K7/8/8/8/8/8 w - - 0 1", 6, 92683},

    //--Self Stalemate
    {"K1k5/8/P7/8/8/8/8/8 w - - 0 1", 6, 2217},

    //--Stalemate & Checkmate
    {"8/k1P5/8/1K6/8/8/8/8 w - - 0 1", 7, 567584},

    //--Stalemate & Checkmate
    {"8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1", 4, 23527},

    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 6, 119060324},

    {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
     5,
     193690690},

    {"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 7, 178633661},

    {"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
     6,
     706045033},

    {"1k6/1b6/8/8/7R/8/8/4K2R b K - 0 1", 5, 1063513},
};

static char *test_perft_hash() {
    Hasher hasher;
    for (PerftTestCase &test : POSITIONS) {
        Position pos(test.fen, hasher);
        std::cout << "perft(`" << test.fen << "`, " << test.depth << ") ";
        bool hash_match = true;
        auto cb = [&](Move move, uint64_t children) {
            Position copy(pos.fen(), hasher);
            hash_match &= (pos.hash() == copy.hash());
        };
        uint64_t nodes = perft(pos, test.depth, test.depth, cb);
        std::cout << nodes << " == " << test.result << "?\n";
        mu_assert("Perft case failed", nodes == test.result);
        mu_assert("Incremental hash failed", hash_match);
    }
    return 0;
}

static char *all_tests() {
    mu_run_test(test_perft_hash);
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