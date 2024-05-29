#include <chrono>
#include <iostream>

#include "Brainiac.hpp"

Brainiac::Bitboard perft(Brainiac::Position &pos,
                         unsigned depth,
                         unsigned max_depth,
                         bool verbose) {
    Brainiac::Bitboard nodes = 0;
    const Brainiac::MoveList &moves = pos.moves();
    if (depth == 1) {
        return moves.size();
    }
    for (const Brainiac::Move &move : moves) {
        pos.make(move);
        uint64_t children = perft(pos, depth - 1, max_depth, verbose);
        pos.undo();
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

    Brainiac::Position pos(fen_string);
    pos.print();
    std::cout << pos.fen() << "\n";

    for (int i = 1; i <= depth; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        std::cout << "Perft(" << i << ") = ";
        uint64_t nodes = perft(pos, i, i, false);
        std::cout << nodes << " (";
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        std::cout << duration.count() / 1000000.0 << " s)\n";
    }
}

void play_bot(Brainiac::Color player_color, unsigned seed) {
    Brainiac::Hasher hasher(seed);
    // Brainiac::Position pos("r5rk/5p1p/5R2/4B3/8/8/7P/7K w - - 0 0", hasher);
    Brainiac::Position pos(Brainiac::DEFAULT_BOARD_FEN, hasher);
    Brainiac::Search bot;

    Brainiac::Move move;
    std::string move_input;
    while (!pos.is_checkmate() && !pos.is_draw()) {
        pos.print();
        std::cout << pos.fen() << "\n";
        short evaluation = evaluate(pos);
        std::cout << "Evaluation: " << evaluation << "\n";
        std::cout << "\n";
        if (evaluation > 1) {
            std::cout << "You are winning\n";
        } else if (evaluation < -1) {
            std::cout << "Opponent winning\n";
        } else {
            std::cout << "Evenly matched\n";
        }

        if (pos.is_check()) {
            std::cout << "Check!\n";
        }
        if (pos.turn() != player_color) {
            Brainiac::Result result = bot.search(pos);
            float nps = result.visited / result.time.count();

            std::cout << "Total time: " << result.time << "\n";
            std::cout << "Visited nodes: " << result.visited << "\n";
            std::cout << "Max depth: " << int(result.max_depth) << "\n";
            std::cout << "Nodes/s: " << nps << "\n";
            pos.make(result.move);
        } else {
            while (move == Brainiac::Move()) {
                std::cout << "Enter a move> ";
                std::cin >> move_input;
                // Undo and redo twice to skip over bot's move as well
                if (move_input == "undo") {
                    for (int i = 0; i < 2; i++) {
                        if (!pos.is_start()) {
                            pos.undo();
                        }
                    }
                    break;
                } else if (move_input == "redo") {
                    for (int i = 0; i < 2; i++) {
                        if (!pos.is_end()) {
                            pos.redo();
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
                    move = pos.find_move(Brainiac::string_to_square(from),
                                         Brainiac::string_to_square(to),
                                         promotion);
                }
            }
            if (move != Brainiac::Move()) {
                pos.make(move);
                move = {};
            }
        }
    }
    pos.print();
    std::cout << pos.fen() << "\n";
    if (pos.is_checkmate()) {
        std::cout << (pos.turn() == Brainiac::Color::White ? "Black" : "White")
                  << " wins!\n";
    } else {
        std::cout << "Draw!\n";
    }
}

int main() {
    Brainiac::Position pos("rnbqkbnr/pppppppp/8/8/8/8/8/8 w KQkq - 0 0");
    // Brainiac::Position pos;

    pos.skip();
    pos.make(Brainiac::Move(Brainiac::E7, Brainiac::E5, Brainiac::PawnDouble));
    pos.skip();

    Brainiac::Value placement = compute_placement(pos.board());
    Brainiac::Value material = compute_material(pos.board());

    pos.print();
    std::cout << pos.fen() << "\n";
    std::cout << "Placement: " << placement << "\n";
    std::cout << "Material: " << material << "\n\n";
    std::cout << "Value: " << Brainiac::evaluate(pos) << "\n";
    play_bot(Brainiac::Color::Black, 1118943);
    return 0;
}