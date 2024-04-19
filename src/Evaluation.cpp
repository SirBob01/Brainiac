#include "Evaluation.hpp"

namespace Brainiac {
    short compute_material(Position &pos) {
        short total = 0;
        for (uint8_t i = 0; i < 12; i++) {
            Piece piece = static_cast<Piece>(i);
            Bitboard board = pos.board().bitboard(piece);
            total += count_set_bitboard(board) * PIECE_WEIGHTS[i];
        }
        return total;
    }
} // namespace Brainiac