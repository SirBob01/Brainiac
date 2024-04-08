#include "MoveGen.hpp"

namespace Brainiac {
    Bitboard MoveGen::get_king_attacks(Bitboard king) {
        return KING_MOVE_MASKS[find_lsb_bitboard(f_king)] & ~friends;
    }

    Bitboard MoveGen::get_pawn_advances(Bitboard pawn) {
        unsigned sq = find_lsb_bitboard(pawn);
        return PAWN_ADVANCE_MASKS[64 * turn + sq] & ~all;
    }

    Bitboard MoveGen::get_pawn_doubles(Bitboard pawn) {
        unsigned sq = find_lsb_bitboard(pawn);
        Bitboard advance_mask = PAWN_ADVANCE_MASKS[64 * turn + sq];
        return get_pawn_advances(advance_mask) & ~all;
    }

    Bitboard MoveGen::get_pawn_captures(Bitboard pawn) {
        unsigned sq = find_lsb_bitboard(pawn);
        return PAWN_CAPTURE_MASKS[64 * turn + sq] & enemies;
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
        Bitboard promote_ranks = RANKS[0] | RANKS[7];
        while (pawns) {
            Bitboard pawn = get_lsb_bitboard(pawns);
            Bitboard advance = get_pawn_advances(pawn) | get_pawn_doubles(pawn);
            Bitboard capture = get_pawn_captures(pawn);
            Square src_sq = static_cast<Square>(find_lsb_bitboard(pawn));

            // Quiet pawn movement
            Bitboard quiet = advance & ~promote_ranks;
            Bitboard promote = advance & promote_ranks;
            while (quiet) {
                Square dst_sq = static_cast<Square>(find_lsb_bitboard(quiet));
                moves.add(src_sq, dst_sq, MoveType::Quiet);
                quiet = pop_lsb_bitboard(quiet);
            }
            while (promote) {
                Square dst_sq = static_cast<Square>(find_lsb_bitboard(promote));
                moves.add(src_sq, dst_sq, MoveType::KnightPromo);
                moves.add(src_sq, dst_sq, MoveType::RookPromo);
                moves.add(src_sq, dst_sq, MoveType::BishopPromo);
                moves.add(src_sq, dst_sq, MoveType::QueenPromo);
                promote = pop_lsb_bitboard(promote);
            }

            // Capture pawn movement
            Bitboard quiet_capture = capture & ~promote_ranks & ~ep;
            Bitboard promote_capture = capture & promote_ranks & ~ep;
            Bitboard ep_capture = capture & ep;
            while (quiet_capture) {
                Square dst_sq =
                    static_cast<Square>(find_lsb_bitboard(quiet_capture));
                moves.add(src_sq, dst_sq, MoveType::Capture);
                quiet_capture = pop_lsb_bitboard(quiet_capture);
            }
            while (promote_capture) {
                Square dst_sq =
                    static_cast<Square>(find_lsb_bitboard(promote_capture));
                moves.add(src_sq, dst_sq, MoveType::KnightPromo);
                moves.add(src_sq, dst_sq, MoveType::RookPromo);
                moves.add(src_sq, dst_sq, MoveType::BishopPromo);
                moves.add(src_sq, dst_sq, MoveType::QueenPromo);
                promote_capture = pop_lsb_bitboard(promote_capture);
            }
            while (ep_capture) {
                Square dst_sq =
                    static_cast<Square>(find_lsb_bitboard(ep_capture));
                moves.add(src_sq, dst_sq, MoveType::EnPassant);
                ep_capture = pop_lsb_bitboard(ep_capture);
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