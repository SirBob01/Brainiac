#include "Game.hpp"

namespace Brainiac {
    Game::Game(std::string fen) {
        _states.reserve(64);
        _states.emplace_back();

        std::vector<std::string> fields = tokenize(fen, ' ');

        _index = 0;

        int row = 7;
        int col = 0;
        for (auto &c : fields[0]) {
            if (c == '/') {
                row--;
                col = 0;
            } else if (c <= '9' && c >= '0') {
                col += c - '0';
            } else {
                int char_idx = 0;
                while (PIECE_CHARS[char_idx] != c) {
                    char_idx++;
                }
                _states[_index].board.set(
                    Square(row * 8 + col),
                    Piece((char_idx / 6) * 6 + (char_idx % 6)));
                col++;
            }
        }

        _states[_index].castling = 0;
        for (auto &c : fields[2]) {
            if (c == 'K') _states[_index].castling |= CastlingRights::WK;
            else if (c == 'Q') _states[_index].castling |= CastlingRights::WQ;
            else if (c == 'k') _states[_index].castling |= CastlingRights::BK;
            else if (c == 'q') _states[_index].castling |= CastlingRights::BQ;
        }

        _states[_index].ep_target = fields[3].length() == 2
                                        ? string_to_square(fields[3])
                                        : Square::Null;
        _states[_index].turn =
            (fields[1][0] == 'w') ? Color::White : Color::Black;
        _states[_index].halfmoves = stoi(fields[4]);
        _states[_index].fullmoves = stoi(fields[5]);

        // Compute the zobrist hash
        _zobrist = _states[_index].compute_hash();
        _states[_index].generate_moves();
    }

    State &Game::push_state() {
        _states.resize(_index + 1);
        _states.emplace_back(_states[_index]);
        _index++;

        State &state = _states[_index];
        state.halfmoves++;
        return state;
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