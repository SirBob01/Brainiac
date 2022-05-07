#include "brainiac.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>

uint64_t perft(brainiac::Board &b, int depth, int max_depth, bool verbose) {
    uint64_t nodes = 0;
    auto moves = b.get_moves();
    if (depth == 1) {
        return moves.size();
    }
    for (auto &move : moves) {
        b.execute_move(move);
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
            move = b.create_move(brainiac::Square(from),
                                 brainiac::Square(to),
                                 promotion);
        }
        b.execute_move(move);
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
            b.execute_move(bot.move(b));
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
                    move = b.create_move(brainiac::Square(from),
                                         brainiac::Square(to),
                                         promotion);
                }
            }
            if (!move.is_invalid()) {
                b.execute_move(move);
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
            } else if (move_input == "stop") {
                return;
            } else {
                std::string from = move_input.substr(0, 2);
                std::string to = move_input.substr(2, 2);
                char promotion = 0;
                if (move_input.length() == 5) {
                    promotion = move_input[4];
                }
                move = b.create_move(brainiac::Square(from),
                                     brainiac::Square(to),
                                     promotion);
            }
        }
        if (!move.is_invalid()) {
            b.execute_move(move);
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