#include <iostream>

#include "Brainiac.hpp"

int main() {
    Brainiac::Game game;
    game.print();
    std::cout << (game.fen()) << "\n";
    Brainiac::print_bitboard(game.hash());
    return 0;
}