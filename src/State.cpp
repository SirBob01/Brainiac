#include "State.hpp"

namespace Brainiac {
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

    uint64_t State::compute_hash() const {
        // TODO: Algorithm to compute the initial zobrist hash
        return 0;
    }

    void State::generate_moves() {
        // TODO: Fast legal move generator
    }

    void State::print() const {
        if (turn == Color::White) std::cout << "White's turn.\n";
        else std::cout << "Black's turn.\n";
        board.print();
    }
} // namespace Brainiac