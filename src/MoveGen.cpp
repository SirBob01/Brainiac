#include "MoveGen.hpp"
#include "Bitboard.hpp"

namespace Brainiac {
    Bitboard MoveGen::get_king_attacks(Bitboard king) {
        return KING_MOVE_MASKS[find_lsb_bitboard(f_king)] & ~friends;
    }

    Bitboard MoveGen::get_pawn_advances(Bitboard pawn) {
        Bitboard mask = 0;
        if (turn == Color::White) {
            mask = (pawn << 8) & 0xffffffffffffff00;
        } else {
            mask = (pawn >> 8) & 0x00ffffffffffffff;
        }
        return mask & ~all;
    }

    Bitboard MoveGen::get_pawn_doubles(Bitboard pawn) {
        Bitboard mask = 0;
        if (turn == Color::White) {
            mask = (pawn << 16) & RANKS[3];
        } else {
            mask = (pawn >> 16) & RANKS[4];
        }
        return mask & ~all;
    }

    Bitboard MoveGen::get_knight_attacks(Bitboard knight) {
        return KNIGHT_MOVE_MASKS[find_lsb_bitboard(knight)] & ~friends;
    }

    Bitboard MoveGen::get_rook_attacks(Bitboard rook) {
        unsigned sq = find_lsb_bitboard(rook);

        const SlidingMoveTable &table = ROOK_ATTACK_TABLES[sq];
        Bitboard blockers = table.block_mask & all;
        Bitboard index = (blockers * table.magic) >> (64 - table.shift);

        return table.move_masks[index] & ~friends;
    }

    Bitboard MoveGen::get_bishop_attacks(Bitboard bishop) {
        unsigned sq = find_lsb_bitboard(bishop);

        const SlidingMoveTable &table = BISHOP_ATTACK_TABLES[sq];
        Bitboard blockers = table.block_mask & all;
        Bitboard index = (blockers * table.magic) >> (64 - table.shift);

        return table.move_masks[index] & ~friends;
    }

    Bitboard MoveGen::get_queen_attacks(Bitboard queen) {
        unsigned sq = find_lsb_bitboard(queen);

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

    bool MoveGen::generate(MoveList &moves) {
        bool check = false;

        Bitboard pawns = f_pawn;
        while (pawns) {
            Bitboard pawn = get_lsb_bitboard(pawns);
            Bitboard targets = get_pawn_advances(pawn) | get_pawn_doubles(pawn);
            Square src_sq = static_cast<Square>(find_lsb_bitboard(pawn));

            // Quiet pawn movement
            while (targets) {
                Square dst_sq = static_cast<Square>(find_lsb_bitboard(targets));
                moves.add(src_sq, dst_sq, MoveType::Quiet);
                targets = pop_lsb_bitboard(targets);
            }

            pawns = pop_lsb_bitboard(pawns);
        }

        Bitboard knights = f_knight;
        while (knights) {
            Bitboard knight = get_lsb_bitboard(knights);
            Bitboard targets = get_knight_attacks(knight);
            Square src_sq = static_cast<Square>(find_lsb_bitboard(knight));

            // Quiet knight movement
            Bitboard quiet = targets & ~enemies;
            while (quiet) {
                Square dst_sq = static_cast<Square>(find_lsb_bitboard(quiet));
                moves.add(src_sq, dst_sq, MoveType::Quiet);
                quiet = pop_lsb_bitboard(quiet);
            }

            // Capture knight movement
            Bitboard capture = targets & enemies;
            while (capture) {
                Square dst_sq = static_cast<Square>(find_lsb_bitboard(capture));
                moves.add(src_sq, dst_sq, MoveType::Capture);
                capture = pop_lsb_bitboard(capture);
            }

            knights = pop_lsb_bitboard(knights);
        }
        return check;
    }
} // namespace Brainiac