#include "Game.hpp"

namespace Brainiac {
    Game::Game(std::string fen) {
        _states.reserve(64);
        _states.emplace_back(fen);
        _index = 0;

        // Compute the initial zobrist hash to be updated on move
        _hash = _states[_index].hash();
        _states[_index].generate_moves();
    }

    std::string Game::fen() const { return _states[_index].fen(); }

    StateHash Game::hash() const { return _hash; }

    const MoveList &Game::moves() const { return _states[_index].moves; }

    bool Game::is_check() const { return _states[_index].check; }

    bool Game::is_checkmate() const {
        return is_check() && moves().size() == 0;
    }

    bool Game::is_statelmate() const {
        return !is_check() && moves().size() == 0;
    }

    void Game::make(Move move) {
        // TODO
    }

    void Game::undo() {
        // TODO
    }

    void Game::skip() {
        // TODO
    }

    void Game::print() const { _states[_index].print(); }
} // namespace Brainiac