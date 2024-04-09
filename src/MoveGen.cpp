#include "MoveGen.hpp"
#include "Bitboard.hpp"

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

    Bitboard MoveGen::attackmask() {
        Bitboard mask = king_attacks(find_lsb_bitboard(o_king));

        Bitboard pawns = o_pawn;
        while (pawns) {
            Square sq = find_lsb_bitboard(pawns);
            mask |= pawn_captures(sq, static_cast<Color>(!turn));
            pawns = pop_lsb_bitboard(pawns);
        }

        Bitboard knights = o_knight;
        while (knights) {
            Square sq = find_lsb_bitboard(knights);
            mask |= knight_attacks(sq);
            knights = pop_lsb_bitboard(knights);
        }

        Bitboard bishops = o_bishop;
        while (bishops) {
            Square sq = find_lsb_bitboard(bishops);
            mask |= bishop_attacks(sq, enemies, friends);
            bishops = pop_lsb_bitboard(bishops);
        }

        Bitboard rooks = o_rook;
        while (rooks) {
            Square sq = find_lsb_bitboard(rooks);
            mask |= rook_attacks(sq, enemies, friends);
            rooks = pop_lsb_bitboard(rooks);
        }

        Bitboard queens = o_queen;
        while (queens) {
            Square sq = find_lsb_bitboard(queens);
            mask |= queen_attacks(sq, enemies, friends);
            queens = pop_lsb_bitboard(queens);
        }
        return mask & ~enemies;
    }

    bool MoveGen::generate(MoveList &list) {
        Bitboard attacks = attackmask();
        return attacks & f_king;
    }
} // namespace Brainiac