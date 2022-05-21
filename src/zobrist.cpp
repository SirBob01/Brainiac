#include "zobrist.h"

namespace brainiac {
    void zobrist_init() {
        std::default_random_engine rng;
        std::uniform_real_distribution<double> uniform(0, UINT64_MAX);

        rng.seed(1118943);
        for (int i = 0; i < PieceType::NPieces * 2 * 64; i++) {
            piece_bitstrings.push_back(uniform(rng));
        }
        for (int i = 0; i < 4; i++) {
            castling_bitstrings.push_back(uniform(rng));
        }
        for (int i = 0; i < 8; i++) {
            en_passant_bitstrings.push_back(uniform(rng));
        }
        turn_bitstring = uniform(rng);
    }

    uint64_t zobrist_hash(Color turn,
                          uint64_t *bitboards,
                          CastlingFlagSet castling_rights,
                          const Square en_passant) {
        // Silently perform initialization
        if (piece_bitstrings.empty()) {
            zobrist_init();
        }

        uint64_t r = 0;

        // Piece position information
        for (int color = Color::White; color <= Color::Black; color++) {
            for (int type = 0; type < PieceType::NPieces; type++) {
                Piece piece = {static_cast<PieceType>(type),
                               static_cast<Color>(color)};
                int index = piece.get_index();
                uint64_t bitboard = bitboards[index];
                for (int i = 0; i < 64; i++) {
                    if (bitboard & 1) {
                        r ^= piece_bitstrings[(i * PieceType::NPieces * 2) +
                                              index];
                    }
                    bitboard >>= 1;
                }
            }
        }

        // Turn information
        if (turn == Color::Black) {
            r ^= turn_bitstring;
        }

        // Castling information
        int castling_rights_index = 0;
        while (castling_rights) {
            if (castling_rights & 1) {
                r ^= castling_bitstrings[castling_rights_index];
            }
            castling_rights_index++;
            castling_rights >>= 1;
        }

        // En passant information
        if (!is_square_invalid(en_passant)) {
            r ^= en_passant_bitstrings[en_passant % 8];
        }
        return r;
    }
} // namespace brainiac