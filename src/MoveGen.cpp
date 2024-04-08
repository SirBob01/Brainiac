#include "MoveGen.hpp"

namespace Brainiac {
    Bitboard MoveGen::king_attacks(Square sq) {
        return KING_MOVE_MASKS[sq] & ~friends;
    }

    Bitboard MoveGen::pawn_advances(Square sq) {
        return PAWN_ADVANCE_MASKS[64 * turn + sq] & ~all;
    }

    Bitboard MoveGen::pawn_doubles(Square sq) {
        Bitboard advance_mask = PAWN_ADVANCE_MASKS[64 * turn + sq];
        return pawn_advances(find_lsb_bitboard(advance_mask)) & ~all;
    }

    Bitboard MoveGen::pawn_captures(Square sq) {
        return PAWN_CAPTURE_MASKS[64 * turn + sq] & enemies;
    }

    Bitboard MoveGen::knight_attacks(Square sq) {
        return KNIGHT_MOVE_MASKS[sq] & ~friends;
    }

    Bitboard MoveGen::rook_attacks(Square sq) {
        const SlidingMoveTable &table = ROOK_ATTACK_TABLES[sq];
        Bitboard blockers = table.block_mask & all;
        Bitboard index = (blockers * table.magic) >> (64 - table.shift);

        return table.move_masks[index] & ~friends;
    }

    Bitboard MoveGen::bishop_attacks(Square sq) {
        const SlidingMoveTable &table = BISHOP_ATTACK_TABLES[sq];
        Bitboard blockers = table.block_mask & all;
        Bitboard index = (blockers * table.magic) >> (64 - table.shift);

        return table.move_masks[index] & ~friends;
    }

    Bitboard MoveGen::queen_attacks(Square sq) {
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

    void MoveGen::generate_pawn_moves(MoveList &moves) {
        Bitboard pawns = f_pawn;
        Bitboard promote_ranks = RANKS[0] | RANKS[7];
        while (pawns) {
            Square src_sq = find_lsb_bitboard(pawns);
            Bitboard advance = pawn_advances(src_sq) | pawn_doubles(src_sq);
            Bitboard capture = pawn_captures(src_sq);

            // Quiet pawn movement
            Bitboard quiet = advance & ~promote_ranks;
            Bitboard promote = advance & promote_ranks;
            while (quiet) {
                Square dst_sq = find_lsb_bitboard(quiet);
                moves.add(src_sq, dst_sq, MoveType::Quiet);
                quiet = pop_lsb_bitboard(quiet);
            }
            while (promote) {
                Square dst_sq = find_lsb_bitboard(promote);
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
                Square dst_sq = find_lsb_bitboard(quiet_capture);
                moves.add(src_sq, dst_sq, MoveType::Capture);
                quiet_capture = pop_lsb_bitboard(quiet_capture);
            }
            while (promote_capture) {
                Square dst_sq = find_lsb_bitboard(promote_capture);
                moves.add(src_sq, dst_sq, MoveType::KnightPromoCapture);
                moves.add(src_sq, dst_sq, MoveType::RookPromoCapture);
                moves.add(src_sq, dst_sq, MoveType::BishopPromoCapture);
                moves.add(src_sq, dst_sq, MoveType::QueenPromoCapture);
                promote_capture = pop_lsb_bitboard(promote_capture);
            }
            while (ep_capture) {
                Square dst_sq = find_lsb_bitboard(ep_capture);
                moves.add(src_sq, dst_sq, MoveType::EnPassant);
                ep_capture = pop_lsb_bitboard(ep_capture);
            }

            pawns = pop_lsb_bitboard(pawns);
        }
    }

    void MoveGen::generate_knight_moves(MoveList &moves) {
        Bitboard knights = f_knight;
        while (knights) {
            Square src_sq = find_lsb_bitboard(knights);
            Bitboard targets = knight_attacks(src_sq);

            // Quiet knight movement
            Bitboard quiet = targets & ~enemies;
            while (quiet) {
                Square dst_sq = find_lsb_bitboard(quiet);
                moves.add(src_sq, dst_sq, MoveType::Quiet);
                quiet = pop_lsb_bitboard(quiet);
            }

            // Capture knight movement
            Bitboard capture = targets & enemies;
            while (capture) {
                Square dst_sq = find_lsb_bitboard(capture);
                moves.add(src_sq, dst_sq, MoveType::Capture);
                capture = pop_lsb_bitboard(capture);
            }

            knights = pop_lsb_bitboard(knights);
        }
    }

    void MoveGen::generate_rook_moves(MoveList &moves) {
        Bitboard rooks = f_rook;
        while (rooks) {
            Square src_sq = find_lsb_bitboard(rooks);
            Bitboard targets = rook_attacks(src_sq);

            // Quiet rook movement
            Bitboard quiet = targets & ~enemies;
            while (quiet) {
                Square dst_sq = find_lsb_bitboard(quiet);
                moves.add(src_sq, dst_sq, MoveType::Quiet);
                quiet = pop_lsb_bitboard(quiet);
            }

            // Capture rook movement
            Bitboard capture = targets & enemies;
            while (capture) {
                Square dst_sq = find_lsb_bitboard(capture);
                moves.add(src_sq, dst_sq, MoveType::Capture);
                capture = pop_lsb_bitboard(capture);
            }

            rooks = pop_lsb_bitboard(rooks);
        }
    }

    void MoveGen::generate_bishop_moves(MoveList &moves) {
        Bitboard bishops = f_bishop;
        while (bishops) {
            Square src_sq = find_lsb_bitboard(bishops);
            Bitboard targets = bishop_attacks(src_sq);

            // Quiet bishop movement
            Bitboard quiet = targets & ~enemies;
            while (quiet) {
                Square dst_sq = find_lsb_bitboard(quiet);
                moves.add(src_sq, dst_sq, MoveType::Quiet);
                quiet = pop_lsb_bitboard(quiet);
            }

            // Capture bishop movement
            Bitboard capture = targets & enemies;
            while (capture) {
                Square dst_sq = find_lsb_bitboard(capture);
                moves.add(src_sq, dst_sq, MoveType::Capture);
                capture = pop_lsb_bitboard(capture);
            }

            bishops = pop_lsb_bitboard(bishops);
        }
    }

    void MoveGen::generate_queen_moves(MoveList &moves) {
        Bitboard queens = f_queen;
        while (queens) {
            Square src_sq = find_lsb_bitboard(queens);
            Bitboard targets = queen_attacks(src_sq);

            // Quiet queen movement
            Bitboard quiet = targets & ~enemies;
            while (quiet) {
                Square dst_sq = find_lsb_bitboard(quiet);
                moves.add(src_sq, dst_sq, MoveType::Quiet);
                quiet = pop_lsb_bitboard(quiet);
            }

            // Capture queen movement
            Bitboard capture = targets & enemies;
            while (capture) {
                Square dst_sq = find_lsb_bitboard(capture);
                moves.add(src_sq, dst_sq, MoveType::Capture);
                capture = pop_lsb_bitboard(capture);
            }

            queens = pop_lsb_bitboard(queens);
        }
    }

    void MoveGen::generate_king_moves(MoveList &moves) {
        Bitboard king = f_king;
        while (king) {
            Square src_sq = find_lsb_bitboard(king);
            Bitboard targets = king_attacks(src_sq);

            // Quiet king movement
            Bitboard quiet = targets & ~enemies;
            while (quiet) {
                Square dst_sq = find_lsb_bitboard(quiet);
                moves.add(src_sq, dst_sq, MoveType::Quiet);
                quiet = pop_lsb_bitboard(quiet);
            }

            // Capture king movement
            Bitboard capture = targets & enemies;
            while (capture) {
                Square dst_sq = find_lsb_bitboard(capture);
                moves.add(src_sq, dst_sq, MoveType::Capture);
                capture = pop_lsb_bitboard(capture);
            }

            king = pop_lsb_bitboard(king);
        }
    }

    bool MoveGen::generate(MoveList &moves) {
        bool check = false;
        generate_pawn_moves(moves);
        generate_knight_moves(moves);
        generate_rook_moves(moves);
        generate_bishop_moves(moves);
        generate_queen_moves(moves);
        generate_king_moves(moves);
        return check;
    }
} // namespace Brainiac