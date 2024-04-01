#include "Game.hpp"

namespace Brainiac {
    Game::Game(std::string fen) {
        _states.reserve(64);
        _states.emplace_back();

        std::vector<std::string> fields = tokenize(fen, ' ');

        _state_index = 0;
        _fullmoves = stoi(fields[5]);
        _turn = (fields[1][0] == 'w') ? Color::White : Color::Black;

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
                _states[_state_index].board.set(
                    Square(row * 8 + col),
                    Piece((char_idx / 6) * 6 + (char_idx % 6)));
                col++;
            }
        }

        _states[_state_index].castling = 0;
        for (auto &c : fields[2]) {
            if (c == 'K') _states[_state_index].castling |= CastlingRights::WK;
            else if (c == 'Q')
                _states[_state_index].castling |= CastlingRights::WQ;
            else if (c == 'k')
                _states[_state_index].castling |= CastlingRights::BK;
            else if (c == 'q')
                _states[_state_index].castling |= CastlingRights::BQ;
        }

        _states[_state_index].ep_target = fields[3].length() == 2
                                              ? string_to_square(fields[3])
                                              : Square::Null;
        _states[_state_index].halfmoves = stoi(fields[4]);

        _states[_state_index].generate_moves(_turn);
        _states[_state_index].compute_hash();
    }

    State &Game::push_state() {
        _states.resize(_state_index + 1);
        _states.emplace_back(_states[_state_index]);
        _state_index++;

        State &state = _states[_state_index];
        state.halfmoves++;
        return state;
    }

    std::string Game::fen() const {
        const State &state = _states[_state_index];
        std::string fen = "";
        for (int row = 7; row >= 0; row--) {
            int counter = 0;
            for (int col = 0; col < 8; col++) {
                Piece piece = state.board.get(Square(row * 8 + col));
                if (piece != Piece::Empty) {
                    if (counter) {
                        fen += counter + '0';
                        counter = 0;
                    }
                    fen += PIECE_CHARS[piece];
                } else {
                    counter += 1;
                }
            }
            if (counter) fen += counter + '0';
            if (row) fen += '/';
        }
        fen += " ";
        fen += _turn == Color::White ? "w" : "b";

        std::string castling_rights = "";

        if (state.castling & CastlingRights::WK) castling_rights += 'K';
        if (state.castling & CastlingRights::WQ) castling_rights += 'Q';
        if (state.castling & CastlingRights::BK) castling_rights += 'k';
        if (state.castling & CastlingRights::BQ) castling_rights += 'q';
        if (castling_rights.length() == 0) castling_rights = "-";
        fen += " " + castling_rights;

        if (state.ep_target == Square::Null) {
            fen += " -";
        } else {
            fen += " ";
            fen += square_to_string(state.ep_target);
        }

        fen += " ";
        fen += std::to_string(state.halfmoves);
        fen += " ";
        fen += std::to_string(_fullmoves);
        return fen;
    }

    const MoveList &Game::moves() const { return _states[_state_index].moves; }

    bool Game::is_check() const { return _states[_state_index].check; }

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

    void Game::print() const {
        const State &current_state = _states[_state_index];
        if (_turn == Color::White) std::cout << "White's turn.\n";
        else std::cout << "Black's turn.\n";
        current_state.board.print();
    }
} // namespace Brainiac