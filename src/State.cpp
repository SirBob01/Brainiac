#include "State.hpp"

namespace Brainiac {
    State::State(std::string fen) {
        std::vector<std::string> fields = tokenize(fen, ' ');
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
                board.set(Square(row * 8 + col),
                          Piece((char_idx / 6) * 6 + (char_idx % 6)));
                col++;
            }
        }

        castling = 0;
        for (auto &c : fields[2]) {
            if (c == 'K') castling |= CastlingRights::WK;
            else if (c == 'Q') castling |= CastlingRights::WQ;
            else if (c == 'k') castling |= CastlingRights::BK;
            else if (c == 'q') castling |= CastlingRights::BQ;
        }

        ep_target = fields[3].length() == 2 ? string_to_square(fields[3])
                                            : Square::Null;
        turn = (fields[1][0] == 'w') ? Color::White : Color::Black;
        halfmoves = stoi(fields[4]);
        fullmoves = stoi(fields[5]);
    }

    std::string State::fen() const {
        std::string fen = "";
        for (int row = 7; row >= 0; row--) {
            int counter = 0;
            for (int col = 0; col < 8; col++) {
                Piece piece = board.get(Square(row * 8 + col));
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
        fen += turn == Color::White ? "w" : "b";

        std::string castling_rights = "";

        if (castling & CastlingRights::WK) castling_rights += 'K';
        if (castling & CastlingRights::WQ) castling_rights += 'Q';
        if (castling & CastlingRights::BK) castling_rights += 'k';
        if (castling & CastlingRights::BQ) castling_rights += 'q';
        if (castling_rights.length() == 0) castling_rights = "-";
        fen += " " + castling_rights;

        if (ep_target == Square::Null) {
            fen += " -";
        } else {
            fen += " ";
            fen += square_to_string(ep_target);
        }

        fen += " ";
        fen += std::to_string(halfmoves);
        fen += " ";
        fen += std::to_string(fullmoves);
        return fen;
    }

    StateHash State::hash() const {
        unsigned bitstring_n = (64 * 12) + 1 + 4;
        if (!ZOBRIST_BITSTRINGS.size()) {
            unsigned seed = time(0); // 123456;
            std::default_random_engine rng{seed};
            std::uniform_real_distribution<double> uniform(0, UINT64_MAX);

            for (unsigned i = 0; i < bitstring_n; i++) {
                ZOBRIST_BITSTRINGS.push_back(uniform(rng));
            }
        }

        StateHash hash = 0;
        if (turn == Color::Black) {
            hash ^= ZOBRIST_BITSTRINGS[bitstring_n - 1];
        }
        for (uint8_t c = 0; c < 4; c++) {
            if (castling & (1 << c)) {
                hash ^= ZOBRIST_BITSTRINGS[bitstring_n - 1 - c];
            }
        }
        for (uint8_t sq = 0; sq < 64; sq++) {
            Piece piece = board.get(Square(sq));
            if (piece != Piece::Empty) {
                hash ^= ZOBRIST_BITSTRINGS[sq * 12 + piece];
            }
        }
        return hash;
    }

    void State::print() const {
        if (turn == Color::White) std::cout << "White's turn.\n";
        else std::cout << "Black's turn.\n";
        board.print();
    }

    void State::generate_moves() {
        // TODO: Fast legal move generator
    }
} // namespace Brainiac