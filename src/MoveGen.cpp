#include "MoveGen.hpp"

namespace Brainiac {
    Bitboard king_attacks(Square sq) { return KING_MOVE_MASKS[sq]; }

    Bitboard pawn_advances(Square sq, Color turn) {
        return PAWN_ADVANCE_MASKS[64 * turn + sq];
    }

    Bitboard pawn_doubles(Square sq, Color turn) {
        return PAWN_DOUBLE_MASKS[64 * turn + sq];
    }

    Bitboard pawn_captures(Square sq, Color turn) {
        return PAWN_CAPTURE_MASKS[64 * turn + sq];
    }

    Bitboard knight_attacks(Square sq) { return KNIGHT_MOVE_MASKS[sq]; }

    Bitboard rook_attacks(Square sq, Bitboard friends, Bitboard enemies) {
        const SlidingMoveTable &table = ROOK_ATTACK_TABLES[sq];
        Bitboard blockers = table.block_mask & (friends | enemies);
        Bitboard index = (blockers * table.magic) >> (64 - table.shift);

        return table.move_masks[index] & ~friends;
    }

    Bitboard bishop_attacks(Square sq, Bitboard friends, Bitboard enemies) {
        const SlidingMoveTable &table = BISHOP_ATTACK_TABLES[sq];
        Bitboard blockers = table.block_mask & (friends | enemies);
        Bitboard index = (blockers * table.magic) >> (64 - table.shift);

        return table.move_masks[index] & ~friends;
    }

    Bitboard queen_attacks(Square sq, Bitboard friends, Bitboard enemies) {
        Bitboard all = friends | enemies;

        const SlidingMoveTable &r_table = ROOK_ATTACK_TABLES[sq];
        unsigned r_shift = 64 - r_table.shift;
        Bitboard r_blockers = r_table.block_mask & all;
        Bitboard r_index = (r_blockers * r_table.magic) >> r_shift;

        const SlidingMoveTable &b_table = BISHOP_ATTACK_TABLES[sq];
        unsigned b_shift = 64 - b_table.shift;
        Bitboard b_blockers = b_table.block_mask & all;
        Bitboard b_index = (b_blockers * b_table.magic) >> b_shift;

        return (r_table.move_masks[r_index] | b_table.move_masks[b_index]) &
               ~friends;
    }
} // namespace Brainiac