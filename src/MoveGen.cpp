#include "MoveGen.hpp"

namespace Brainiac {
    Bitboard MoveGen::get_king_attacks() {
        return KING_MOVE_MASKS[find_lsb_bitboard(f_king)];
    }

    Bitboard MoveGen::get_pawn_advances() {
        Bitboard mask = 0;
        if (turn == Color::White) {
            mask = (f_pawn << 8) & 0xffffffffffffff00;
        } else {
            mask = (f_pawn >> 8) & 0x00ffffffffffffff;
        }
        return mask & ~all;
    }

    Bitboard MoveGen::get_pawn_doubles() {
        Bitboard mask = 0;
        if (turn == Color::White) {
            mask = (f_pawn << 16) & RANKS[3];
        } else {
            mask = (f_pawn >> 16) & RANKS[4];
        }
        return mask & ~all;
    }

    Bitboard MoveGen::get_knight_attacks() {
        Bitboard mask = 0;
        Bitboard pieces = f_knight;

        while (pieces) {
            unsigned sq = find_lsb_bitboard(pieces);
            mask |= KNIGHT_MOVE_MASKS[sq];
            pieces = pop_lsb_bitboard(pieces);
        }
        return mask;
    }

    Bitboard MoveGen::get_rook_attacks() {
        Bitboard mask = 0;
        Bitboard pieces = f_rook;

        while (pieces) {
            unsigned sq = find_lsb_bitboard(pieces);

            const SlidingMoveTable &table = ROOK_ATTACK_TABLES[sq];
            Bitboard blockers = table.block_mask & all;
            Bitboard index = (blockers * table.magic) >> (64 - table.shift);
            mask |= table.move_masks[index];

            pieces = pop_lsb_bitboard(pieces);
        }
        return mask & ~friends;
    }

    Bitboard MoveGen::bishop_attacks() {
        Bitboard mask = 0;
        Bitboard pieces = f_bishop;

        while (pieces) {
            unsigned sq = find_lsb_bitboard(pieces);

            const SlidingMoveTable &table = BISHOP_ATTACK_TABLES[sq];
            Bitboard blockers = table.block_mask & all;
            Bitboard index = (blockers * table.magic) >> (64 - table.shift);
            mask |= table.move_masks[index];

            pieces = pop_lsb_bitboard(pieces);
        }
        return mask & ~friends;
    }

    Bitboard MoveGen::get_queen_attacks() {
        Bitboard mask = 0;
        Bitboard pieces = f_queen;

        while (pieces) {
            unsigned sq = find_lsb_bitboard(pieces);

            const SlidingMoveTable &r_table = ROOK_ATTACK_TABLES[sq];
            unsigned r_shift = 64 - r_table.shift;
            Bitboard r_blockers = r_table.block_mask & all;
            Bitboard r_index = (r_blockers * r_table.magic) >> r_shift;

            const SlidingMoveTable &b_table = BISHOP_ATTACK_TABLES[sq];
            unsigned b_shift = 64 - b_table.shift;
            Bitboard b_blockers = b_table.block_mask & all;
            Bitboard b_index = (b_blockers * b_table.magic) >> b_shift;

            mask |= r_table.move_masks[r_index];
            mask |= b_table.move_masks[b_index];

            pieces = pop_lsb_bitboard(pieces);
        }
        return mask & ~friends;
    }

    bool generate(MoveList &moves) {
        bool check = false;
        return check;
    }
} // namespace Brainiac