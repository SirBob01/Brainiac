#include "Game.hpp"

namespace Brainiac {
    Game::Game(std::string fen, Hasher hasher) {
        _states.reserve(64);
        _states.emplace_back(fen, hasher);
        _index = 0;
        _hasher = hasher;
    }

    State &Game::push_state() {
        _states.resize(_index + 1);
        _states.emplace_back(_states[_index]);
        _index++;

        _states[_index].halfmoves++;
        return _states[_index];
    }

    std::string Game::fen() const { return _states[_index].fen(); }

    Hash Game::hash() const { return _states[_index].hash; }

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

    void Game::undo() { _index--; }

    void Game::skip() {
        State &state = push_state();

        // Update state
        state.fullmoves += (state.turn == Color::Black);
        state.turn = static_cast<Color>(!state.turn);
        state.hash ^= _hasher.bitstring(state.turn);
        state.generate_moves();
    }

    void Game::print() const { _states[_index].print(); }
} // namespace Brainiac