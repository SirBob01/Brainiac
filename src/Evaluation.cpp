#include "Evaluation.hpp"

namespace Brainiac {
    short compute_material(const Board &board) {
        short total = 0;
        for (uint8_t i = 0; i < 12; i++) {
            Piece piece = static_cast<Piece>(i);
            Bitboard bitboard = board.bitboard(piece);
            total += count_set_bitboard(bitboard) * PIECE_WEIGHTS[i];
        }
        return total;
    }

    short evaluate(Position &pos) {
        short sign = (pos.turn() << 1) - 1;

        // Leaf node
        if (pos.is_checkmate()) {
            return -sign * UPPER_BOUND;
        }
        if (pos.is_draw()) {
            return 0;
        }

        // Non-leaf node (depth capped)
        const Board &board = pos.board();
        return sign * compute_material(board);
    }
} // namespace Brainiac