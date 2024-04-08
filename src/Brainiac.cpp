#include <chrono>
#include <iostream>

#include "Brainiac.hpp"
#include "MoveList.hpp"

Brainiac::Bitboard
perft(Brainiac::Game &game, unsigned depth, unsigned max_depth, bool verbose) {
    Brainiac::Bitboard nodes = 0;
    const Brainiac::MoveList &moves = game.moves();
    if (depth == 1) {
        return moves.size();
    }
    for (const Brainiac::Move &move : moves) {
        game.make(move);
        uint64_t children = perft(game, depth - 1, max_depth, verbose);
        game.undo();
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

    Brainiac::Game game(fen_string);
    game.print();
    std::cout << game.fen() << "\n";

    for (int i = 1; i <= depth; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        std::cout << "Perft(" << i << ") = ";
        uint64_t nodes = perft(game, i, i, false);
        std::cout << nodes << " (";
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        std::cout << duration.count() / 1000000.0 << " s)\n";
    }
}

int main() {
    Brainiac::Game game;
    game.print();
    std::cout << "Moves: " << game.moves().size() << "\n";
    for (const Brainiac::Move &move : game.moves()) {
        std::cout << move.standard_notation() << "\n";
    }
    std::cout << (game.fen()) << "\n";
    Brainiac::print_bitboard(game.hash());

    perft_command();
    return 0;
}