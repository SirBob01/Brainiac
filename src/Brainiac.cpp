#include <iostream>

#include "Brainiac.hpp"
#include "Move.hpp"

int main() {
    Brainiac::Game game;
    game.print();
    std::cout << "Moves: " << game.moves().size() << "\n";
    for (const Brainiac::Move &move : game.moves()) {
        std::cout << move.standard_notation() << "\n";
    }
    std::cout << (game.fen()) << "\n";
    Brainiac::print_bitboard(game.hash());
    return 0;
}