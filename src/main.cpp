#include "brainiac.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>

struct PerftTestCase {
    std::string fen;
    int depth;
    int result;
};

std::vector<PerftTestCase> debug_fen = {
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

uint64_t perft(brainiac::Board &b, int depth, int max_depth, bool verbose) {
    uint64_t nodes = 0;
    auto moves = b.get_moves();
    if (depth == 1) {
        return moves.size();
    }
    for (auto &move : moves) {
        b.make_move(move);
        uint64_t children = perft(b, depth - 1, max_depth, verbose);
        b.undo_move();
        if (depth == max_depth && verbose) {
            std::cout << move.standard_notation() << ": " << children << "\n";
        }
        nodes += children;
    }
    return nodes;
}

void perft_command(
    std::string fen_string =
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
    int depth;
    std::cout << "Enter perft depth: ";
    std::cin >> depth;

    brainiac::Board b(fen_string);
    b.print();
    std::cout << b.generate_fen() << "\n";

    for (int i = 1; i <= depth; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        std::cout << "Perft(" << i << ") = ";
        uint64_t nodes = perft(b, i, i, false);
        std::cout << nodes << " (";
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        std::cout << duration.count() / 1000000.0 << " s)\n";
    }
}

void run_tests() {
    for (auto &test_case : debug_fen) {
        std::cout << "perft(`" << test_case.fen << "`, " << test_case.depth
                  << ")...\n";
        brainiac::Board b(test_case.fen);
        int nodes = perft(b, test_case.depth, test_case.depth, false);
        if (nodes != test_case.result) {
            std::cout << "Test failed!\n";
        }
        std::cout << nodes << " " << test_case.result << "\n\n";
    }
}

void debug_command(
    std::string fen_string =
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
    int depth;
    // std::string fen_string;
    // std::cout << "Enter FEN string: ";
    // std::cin >> fen_string;
    std::cout << "Enter perft depth: ";
    std::cin >> depth;

    brainiac::Board b(fen_string);
    b.print();
    std::cout << b.generate_fen() << "\n";

    std::string move_input;
    brainiac::Move move = {};
    while (depth) {
        uint64_t nodes = perft(b, depth, depth, true);
        if (depth == 1) {
            for (auto &m : b.get_moves()) {
                std::cout << m.standard_notation() << "\n";
            }
        }
        std::cout << "Evaluated " << nodes << " nodes\n";

        while (move.is_invalid()) {
            std::cout << "Enter move to visit subtree> ";
            std::cin >> move_input;
            std::string from = move_input.substr(0, 2);
            std::string to = move_input.substr(2, 2);
            char promotion = 0;
            if (move_input.length() == 5) {
                promotion = move_input[4];
            }
            move = b.create_move(brainiac::string_to_square(from),
                                 brainiac::string_to_square(to),
                                 promotion);
        }
        b.make_move(move);
        b.print();
        std::cout << b.generate_fen() << "\n";
        move = {};
        depth--;
    }
}

void play_bot(brainiac::Color player_color) {
    brainiac::Board b;
    brainiac::Search bot;

    brainiac::Move move;
    std::string move_input;
    while (!b.is_checkmate() && !b.is_draw()) {
        b.print();
        std::cout << b.generate_fen() << "\n";
        float evaluation = bot.evaluate(b);
        std::cout << "Evaluation: " << evaluation << "\n";
        std::cout << "PV: ";
        for (auto move : bot.get_principal_variation(b)) {
            std::cout << move.standard_notation() << " ";
        }
        std::cout << "\n";
        if (evaluation > 1) {
            std::cout << "White winning\n";
        } else if (evaluation < -1) {
            std::cout << "Black winning\n";
        } else {
            std::cout << "White and black are evenly matched\n";
        }

        if (b.is_check()) {
            std::cout << "Check!\n";
        }
        if (b.get_turn() != player_color) {
            b.make_move(bot.move(b));
        } else {
            while (move.is_invalid()) {
                std::cout << "Enter a move> ";
                std::cin >> move_input;
                // Undo and redo twice to skip over bot's move as well
                if (move_input == "undo") {
                    for (int i = 0; i < 2; i++) {
                        if (!b.is_initial()) {
                            b.undo_move();
                        }
                    }
                    break;
                } else if (move_input == "redo") {
                    for (int i = 0; i < 2; i++) {
                        if (!b.is_latest()) {
                            b.redo_move();
                        }
                    }
                    break;
                } else if (move_input == "stop") {
                    return;
                } else {
                    std::string from = move_input.substr(0, 2);
                    std::string to = move_input.substr(2, 2);
                    char promotion = 0;
                    if (move_input.length() == 5) {
                        promotion = move_input[4];
                    }
                    move = b.create_move(brainiac::string_to_square(from),
                                         brainiac::string_to_square(to),
                                         promotion);
                }
            }
            if (!move.is_invalid()) {
                b.make_move(move);
                move = {};
            }
        }
    }
    b.print();
    std::cout << b.generate_fen() << "\n";
    if (b.is_checkmate()) {
        std::cout << (b.get_turn() == brainiac::Color::White ? "Black"
                                                             : "White")
                  << " wins!\n";
    } else {
        std::cout << "Draw!\n";
    }
}

void play_command() {
    brainiac::Board b;
    brainiac::Move move;
    std::string move_input;
    while (!b.is_checkmate() && !b.is_draw()) {
        b.print();
        std::cout << b.generate_fen() << "\n";
        if (b.is_check()) {
            std::cout << "Check!\n";
        }
        while (move.is_invalid()) {
            std::cout << "Enter a move> ";
            std::cin >> move_input;
            if (move_input == "undo") {
                if (!b.is_initial()) {
                    b.undo_move();
                    break;
                }
            } else if (move_input == "redo") {
                if (!b.is_latest()) {
                    b.redo_move();
                    break;
                }
            } else if (move_input == "skip") {
                b.skip_move();
                break;
            } else if (move_input == "stop") {
                return;
            } else {
                std::string from = move_input.substr(0, 2);
                std::string to = move_input.substr(2, 2);
                char promotion = 0;
                if (move_input.length() == 5) {
                    promotion = move_input[4];
                }
                move = b.create_move(brainiac::string_to_square(from),
                                     brainiac::string_to_square(to),
                                     promotion);
            }
        }
        if (!move.is_invalid()) {
            b.make_move(move);
            move = {};
        }
    }
    b.print();
    std::cout << b.generate_fen() << "\n";
    if (b.is_checkmate()) {
        std::cout << (b.get_turn() == brainiac::Color::White ? "Black"
                                                             : "White")
                  << " wins!\n";
    } else {
        std::cout << "Draw!\n";
    }
}

void help() {
    std::vector<std::pair<std::string, std::string>> commands;
    commands.push_back(
        std::make_pair("perft [fen_string?]", "Test performance"));
    commands.push_back(
        std::make_pair("debug [fen_string?]", "Debug the chess engine"));
    commands.push_back(std::make_pair("play", "Run a PvP game of chess"));
    commands.push_back(std::make_pair("bot [w|b]", "Play against Brainiac"));
    commands.push_back(std::make_pair("quit", "Quit Brainiac"));

    for (auto &pair : commands) {
        std::cout << std::left << std::setw(20) << pair.first << std::setw(4)
                  << " -- " << std::setw(20) << pair.second << "\n";
    }
}

int main() {
    brainiac::init();

    std::cout << "Chess Engine C++ v.1.0\n";
    std::cout << "Enter 'help' for the list of commands.\n";
    std::string command;
    while (true) {
        std::cout << "Enter command> ";
        std::getline(std::cin, command);
        auto tokens = brainiac::util::tokenize(command, ' ');
        if (tokens.empty()) {
            continue;
        }
        if (tokens[0] == "perft") {
            if (tokens.size() == 2) {
                perft_command(tokens[1]);
            } else {
                perft_command();
            }
        } else if (tokens[0] == "debug") {
            if (tokens.size() == 2) {
                debug_command(tokens[1]);
            } else {
                debug_command();
            }
            debug_command();
        } else if (tokens[0] == "test") {
            run_tests();
        } else if (tokens[0] == "play") {
            play_command();
        } else if (tokens[0] == "bot") {
            if (tokens.size() == 2) {
                if (tokens[1] == "w") {
                    play_bot(brainiac::Color::White);
                } else if (tokens[1] == "b") {
                    play_bot(brainiac::Color::Black);
                } else {
                    std::cout << "Please select your color (w/b).\n";
                }
            } else {
                std::cout << "Please select your color (w/b).\n";
            }
        } else if (tokens[0] == "help") {
            help();
        } else if (tokens[0] == "quit") {
            break;
        }
    }
    return 0;
}