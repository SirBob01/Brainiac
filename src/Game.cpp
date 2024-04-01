#include "Game.hpp"

namespace Brainiac {
    Game::Game(std::string fen) {
        _states.reserve(64);
        _states.emplace_back(fen);
        _index = 0;

        // Compute the zobrist hash of initial state
        _zobrist = _states[_index].compute_hash();
        _states[_index].generate_moves();
    }

    std::string Game::fen() const { return _states[_index].fen(); }

    uint64_t Game::zobrist() const { return _zobrist; }

    const MoveList &Game::moves() const { return _states[_index].moves; }

    bool Game::is_check() const { return _states[_index].check; }

    bool Game::is_checkmate() const {
        return is_check() && moves().size() == 0;
    }

    bool Game::is_statelmate() const {
        return !is_check() && moves().size() == 0;
    }

    void Game::make_move(Move move) {
        // TODO
    }

    void Game::undo_move() {
        // TODO
    }

    void Game::skip_move() {
        // TODO
    }

    void Game::print() const { _states[_index].print(); }
} // namespace Brainiac