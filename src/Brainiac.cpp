#include <chrono>
#include <iostream>

#include "Brainiac.hpp"
#include "MoveList.hpp"

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

int main() {
    Brainiac::Position pos;
    pos.print();
    std::cout << "Moves: " << pos.moves().size() << "\n";
    for (const Brainiac::Move &move : pos.moves()) {
        std::cout << move.standard_notation() << "\n";
    }
    std::cout << (pos.fen()) << "\n";
    Brainiac::print_bitboard(pos.hash());

    perft_command();
    return 0;
}