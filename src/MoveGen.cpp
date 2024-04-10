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

    void MoveGen::compute_attackmask() {
        Color op = static_cast<Color>(!turn);
        o_king_attacks = king_attacks(find_lsb_bitboard(o_king));

        o_pawn_attacks = 0;
        Bitboard pawns = o_pawn;
        while (pawns) {
            Square sq = find_lsb_bitboard(pawns);
            o_pawn_attacks |= pawn_captures(sq, op);
            pawns = pop_lsb_bitboard(pawns);
        }

        o_knight_attacks = 0;
        Bitboard knights = o_knight;
        while (knights) {
            Square sq = find_lsb_bitboard(knights);
            o_knight_attacks |= knight_attacks(sq);
            knights = pop_lsb_bitboard(knights);
        }

        o_rook_h_attacks = 0;
        o_rook_v_attacks = 0;
        Bitboard rooks = o_rook;
        while (rooks) {
            Square sq = find_lsb_bitboard(rooks);
            Bitboard mask = rook_attacks(sq, enemies, friends);
            o_rook_h_attacks |= (mask & SQUARE_RANKS[sq]);
            o_rook_v_attacks |= (mask & SQUARE_FILES[sq]);
            rooks = pop_lsb_bitboard(rooks);
        }

        o_bishop_d1_attacks = 0;
        o_bishop_d2_attacks = 0;
        Bitboard bishops = o_bishop;
        while (bishops) {
            Square sq = find_lsb_bitboard(bishops);
            Bitboard mask = bishop_attacks(sq, enemies, friends);
            o_bishop_d1_attacks |= (mask & SQUARE_DIAGONALS[sq]);
            o_bishop_d2_attacks |= (mask & SQUARE_ANTI_DIAGONALS[sq]);
            bishops = pop_lsb_bitboard(bishops);
        }

        o_queen_h_attacks = 0;
        o_queen_v_attacks = 0;
        o_queen_d1_attacks = 0;
        o_queen_d2_attacks = 0;
        Bitboard queens = o_queen;
        while (queens) {
            Square sq = find_lsb_bitboard(queens);
            Bitboard mask = queen_attacks(sq, enemies, friends);
            o_queen_h_attacks |= (mask & SQUARE_RANKS[sq]);
            o_queen_v_attacks |= (mask & SQUARE_FILES[sq]);
            o_queen_d1_attacks |= (mask & SQUARE_DIAGONALS[sq]);
            o_queen_d2_attacks |= (mask & SQUARE_ANTI_DIAGONALS[sq]);
            queens = pop_lsb_bitboard(queens);
        }

        attackmask =
            (o_king_attacks | o_pawn_attacks | o_knight_attacks |
             o_rook_h_attacks | o_rook_v_attacks | o_bishop_d1_attacks |
             o_bishop_d2_attacks | o_queen_h_attacks | o_queen_v_attacks |
             o_queen_d1_attacks | o_queen_d2_attacks) &
            ~enemies;
        check = attackmask & f_king;
    }

    void MoveGen::compute_checkmask() {
        checkmask = -1;
        if (check) {
            Square sq = find_lsb_bitboard(f_king);
            Bitboard king_slider = queen_attacks(sq, friends, enemies);
            Bitboard king_h = SQUARE_RANKS[sq];
            Bitboard king_v = SQUARE_FILES[sq];
            Bitboard king_d1 = SQUARE_DIAGONALS[sq];
            Bitboard king_d2 = SQUARE_ANTI_DIAGONALS[sq];

            Bitboard h_mask =
                o_rook | o_rook_h_attacks | o_queen | o_queen_h_attacks;
            Bitboard v_mask =
                o_rook | o_rook_v_attacks | o_queen | o_queen_v_attacks;
            Bitboard d1_mask =
                o_bishop | o_bishop_d1_attacks | o_queen | o_queen_d1_attacks;
            Bitboard d2_mask =
                o_bishop | o_bishop_d2_attacks | o_queen | o_queen_d2_attacks;
            checkmask =
                // Pawn
                (pawn_captures(sq, turn) & o_pawn) |
                // Knight
                (knight_attacks(sq) & o_knight) |
                // HV sliders
                (king_slider & king_h & h_mask) |
                (king_slider & king_v & v_mask) |
                // D12 sliders
                (king_slider & king_d1 & d1_mask) |
                (king_slider & king_d2 & d2_mask);
        }
    }

    void MoveGen::compute_pinmasks() {
        Square sq = find_lsb_bitboard(f_king);
        Bitboard king_slider = queen_attacks(sq, 0, enemies);
        std::array<Bitboard, 8> pinmasks = {
            SQUARE_NORTH_RAY[sq] & king_slider,
            SQUARE_SOUTH_RAY[sq] & king_slider,
            SQUARE_EAST_RAY[sq] & king_slider,
            SQUARE_WEST_RAY[sq] & king_slider,

            SQUARE_NORTHEAST_RAY[sq] & king_slider,
            SQUARE_SOUTHWEST_RAY[sq] & king_slider,
            SQUARE_NORTHWEST_RAY[sq] & king_slider,
            SQUARE_SOUTHEAST_RAY[sq] & king_slider,
        };

        Bitboard o_hv = o_rook | o_queen;
        for (unsigned i = 0; i < 4; i++) {
            bool pin = (pinmasks[i] & o_hv) &&
                       !pop_lsb_bitboard(pinmasks[i] & friends);
            pinmasks[i] &= -pin;
        }

        Bitboard o_d12 = o_bishop | o_queen;
        for (unsigned i = 4; i < 8; i++) {
            bool pin = (pinmasks[i] & o_d12) &&
                       !pop_lsb_bitboard(pinmasks[i] & friends);
            pinmasks[i] &= -pin;
        }

        pinmask_h = pinmasks[2] | pinmasks[3];
        pinmask_v = pinmasks[0] | pinmasks[1];
        pinmask_d1 = pinmasks[4] | pinmasks[5];
        pinmask_d2 = pinmasks[6] | pinmasks[7];
        pinmask = pinmask_h | pinmask_v | pinmask_d1 | pinmask_d2;
    }

    void MoveGen::generate_king_moves(MoveList &moves) {
        Bitboard danger = o_pawn_attacks | o_knight_attacks | o_king_attacks;
        Bitboard no_king = friends & ~f_king;

        Bitboard bishops = o_bishop;
        while (bishops) {
            Square sq = find_lsb_bitboard(bishops);
            danger |= bishop_attacks(sq, 0, no_king);
            bishops = pop_lsb_bitboard(bishops);
        }

        Bitboard rooks = o_rook;
        while (bishops) {
            Square sq = find_lsb_bitboard(rooks);
            danger |= rook_attacks(sq, 0, no_king);
            rooks = pop_lsb_bitboard(rooks);
        }

        Bitboard queens = o_queen;
        while (queens) {
            Square sq = find_lsb_bitboard(queens);
            danger |= queen_attacks(sq, 0, no_king);
            queens = pop_lsb_bitboard(queens);
        }

        Square src_sq = find_lsb_bitboard(f_king);
        Bitboard targets = king_attacks(src_sq) & ~(friends | danger);

        // King quiet
        Bitboard quiet = targets & ~enemies;
        while (quiet) {
            Square dst_sq = find_lsb_bitboard(quiet);
            moves.add(src_sq, dst_sq, MoveType::Quiet);
            quiet = pop_lsb_bitboard(quiet);
        }

        // King captures
        Bitboard captures = targets & enemies;
        while (captures) {
            Square dst_sq = find_lsb_bitboard(quiet);
            moves.add(src_sq, dst_sq, MoveType::Capture);
            captures = pop_lsb_bitboard(captures);
        }

        // King castling
        if (!check) {
            CastlingRight king_side = static_cast<CastlingRight>(2 * turn);
            Bitboard king_pass = CASTLING_MASKS[king_side];
            if (!check && (castling & (1 << king_side)) &&
                !(king_pass & attackmask) && !(king_pass & all)) {
                Square dst_sq = static_cast<Square>(src_sq + 2);
                moves.add(src_sq, dst_sq, MoveType::KingCastle);
            }

            CastlingRight queen_side = static_cast<CastlingRight>(2 * turn + 1);
            Bitboard queen_pass = CASTLING_MASKS[queen_side];
            if (!check && (castling & (1 << queen_side)) &&
                !(queen_pass & attackmask) && !(queen_pass & all)) {
                Square dst_sq = static_cast<Square>(src_sq - 2);
                moves.add(src_sq, dst_sq, MoveType::QueenCastle);
            }
        }
    }

    void MoveGen::generate_pawn_moves(MoveList &moves) {
        Bitboard pinmask_d12 = pinmask_d1 | pinmask_d2;
        Bitboard promo_ranks = RANKS[0] | RANKS[7];

        // Masks
        Bitboard advance_unpin = checkmask & ~all;
        Bitboard capture_unpin = checkmask & enemies;
        Bitboard advance_pin = advance_unpin & pinmask_v;
        Bitboard capture_pin = capture_unpin & pinmask_d12;

        Bitboard pawns_pinned = f_pawn & pinmask;
        while (pawns_pinned) {
            Square src_sq = find_lsb_bitboard(pawns_pinned);

            // Pawn advances with promotions
            Bitboard advances = pawn_advances(src_sq, turn) & advance_pin;
            Bitboard advances_only = advances & ~promo_ranks;
            Bitboard advances_promo = advances & promo_ranks;
            while (advances_only) {
                Square dst_sq = find_lsb_bitboard(advances_only);
                moves.add(src_sq, dst_sq, MoveType::Quiet);
                advances_only = pop_lsb_bitboard(advances_only);
            }
            while (advances_promo) {
                Square dst_sq = find_lsb_bitboard(advances_promo);
                moves.add(src_sq, dst_sq, MoveType::KnightPromo);
                moves.add(src_sq, dst_sq, MoveType::RookPromo);
                moves.add(src_sq, dst_sq, MoveType::BishopPromo);
                moves.add(src_sq, dst_sq, MoveType::QueenPromo);
                advances_promo = pop_lsb_bitboard(advances_promo);
            }

            // Pawn double advance
            Bitboard double_mask = -static_cast<bool>(advances) & advance_pin;
            Bitboard doubles = pawn_doubles(src_sq, turn) & double_mask;
            while (doubles) {
                Square dst_sq = find_lsb_bitboard(doubles);
                moves.add(src_sq, dst_sq, MoveType::PawnDouble);
                doubles = pop_lsb_bitboard(doubles);
            }

            // Pawn captures with promotions and en-passant
            Bitboard captures = pawn_captures(src_sq, turn) & capture_pin;
            Bitboard captures_only = captures & ~ep & ~promo_ranks;
            Bitboard captures_ep = captures & ep;
            Bitboard captures_promo = captures & promo_ranks;
            while (captures_only) {
                Square dst_sq = find_lsb_bitboard(captures_only);
                moves.add(src_sq, dst_sq, MoveType::Capture);
                captures_only = pop_lsb_bitboard(captures_only);
            }
            while (captures_ep) {
                Square dst_sq = find_lsb_bitboard(captures_ep);
                moves.add(src_sq, dst_sq, MoveType::EnPassant);
                captures_ep = pop_lsb_bitboard(captures_ep);
            }
            while (captures_promo) {
                Square dst_sq = find_lsb_bitboard(captures_promo);
                moves.add(src_sq, dst_sq, MoveType::KnightPromoCapture);
                moves.add(src_sq, dst_sq, MoveType::RookPromoCapture);
                moves.add(src_sq, dst_sq, MoveType::BishopPromoCapture);
                moves.add(src_sq, dst_sq, MoveType::QueenPromoCapture);
                captures_promo = pop_lsb_bitboard(captures_promo);
            }

            pawns_pinned = pop_lsb_bitboard(pawns_pinned);
        }

        Bitboard pawns_unpinned = f_pawn & ~pinmask;
        while (pawns_unpinned) {
            Square src_sq = find_lsb_bitboard(pawns_unpinned);

            // Pawn advances with promotions
            Bitboard advances = pawn_advances(src_sq, turn) & advance_unpin;
            Bitboard advances_only = advances & ~promo_ranks;
            Bitboard advances_promo = advances & promo_ranks;
            while (advances_only) {
                Square dst_sq = find_lsb_bitboard(advances_only);
                moves.add(src_sq, dst_sq, MoveType::Quiet);
                advances_only = pop_lsb_bitboard(advances_only);
            }
            while (advances_promo) {
                Square dst_sq = find_lsb_bitboard(advances_promo);
                moves.add(src_sq, dst_sq, MoveType::KnightPromo);
                moves.add(src_sq, dst_sq, MoveType::RookPromo);
                moves.add(src_sq, dst_sq, MoveType::BishopPromo);
                moves.add(src_sq, dst_sq, MoveType::QueenPromo);
                advances_promo = pop_lsb_bitboard(advances_promo);
            }

            // Pawn doubles
            Bitboard double_mask = -static_cast<bool>(advances) & advance_unpin;
            Bitboard doubles = pawn_doubles(src_sq, turn) & double_mask;
            while (doubles) {
                Square dst_sq = find_lsb_bitboard(doubles);
                moves.add(src_sq, dst_sq, MoveType::PawnDouble);
                doubles = pop_lsb_bitboard(doubles);
            }

            // Pawn captures with promotions and en-passant
            Bitboard captures = pawn_captures(src_sq, turn) & capture_unpin;
            Bitboard captures_only = captures & ~ep & ~promo_ranks;
            Bitboard captures_ep = captures & ep;
            Bitboard captures_promo = captures & promo_ranks;
            while (captures_only) {
                Square dst_sq = find_lsb_bitboard(captures_only);
                moves.add(src_sq, dst_sq, MoveType::Capture);
                captures_only = pop_lsb_bitboard(captures_only);
            }
            while (captures_ep) {
                Square dst_sq = find_lsb_bitboard(captures_ep);
                moves.add(src_sq, dst_sq, MoveType::EnPassant);
                captures_ep = pop_lsb_bitboard(captures_ep);
            }
            while (captures_promo) {
                Square dst_sq = find_lsb_bitboard(captures_promo);
                moves.add(src_sq, dst_sq, MoveType::KnightPromoCapture);
                moves.add(src_sq, dst_sq, MoveType::RookPromoCapture);
                moves.add(src_sq, dst_sq, MoveType::BishopPromoCapture);
                moves.add(src_sq, dst_sq, MoveType::QueenPromoCapture);
                captures_promo = pop_lsb_bitboard(captures_promo);
            }

            pawns_unpinned = pop_lsb_bitboard(pawns_unpinned);
        }
    }

    void MoveGen::generate_knight_moves(MoveList &moves) {
        Bitboard knights = f_knight & ~pinmask;
        while (knights) {
            Square src_sq = find_lsb_bitboard(knights);
            Bitboard targets = knight_attacks(src_sq) & (~friends & checkmask);

            // Knight quiet
            Bitboard quiet = targets & ~enemies;
            while (quiet) {
                Square dst_sq = find_lsb_bitboard(quiet);
                moves.add(src_sq, dst_sq, MoveType::Quiet);
                quiet = pop_lsb_bitboard(quiet);
            }

            // Knight captures
            Bitboard captures = targets & enemies;
            while (captures) {
                Square dst_sq = find_lsb_bitboard(captures);
                moves.add(src_sq, dst_sq, MoveType::Capture);
                captures = pop_lsb_bitboard(captures);
            }

            knights = pop_lsb_bitboard(knights);
        }
    }

    void MoveGen::generate_rook_moves(MoveList &moves) {
        // Pinned rooks
        Bitboard rooks_pinned = f_rook & pinmask;
        while (rooks_pinned) {
            Square src_sq = find_lsb_bitboard(rooks_pinned);
            Bitboard targets =
                rook_attacks(src_sq, friends, enemies) & checkmask & pinmask;

            // Rook quiet
            Bitboard quiet = targets & ~enemies;
            while (quiet) {
                Square dst_sq = find_lsb_bitboard(quiet);
                moves.add(src_sq, dst_sq, MoveType::Quiet);
                quiet = pop_lsb_bitboard(quiet);
            }

            // Rook captures
            Bitboard captures = targets & enemies;
            while (captures) {
                Square dst_sq = find_lsb_bitboard(captures);
                moves.add(src_sq, dst_sq, MoveType::Capture);
                captures = pop_lsb_bitboard(captures);
            }

            rooks_pinned = pop_lsb_bitboard(rooks_pinned);
        }

        // Unpinned rooks
        Bitboard rooks_unpinned = f_rook & ~pinmask;
        while (rooks_unpinned) {
            Square src_sq = find_lsb_bitboard(rooks_unpinned);
            Bitboard targets =
                rook_attacks(src_sq, friends, enemies) & checkmask;

            // Rook quiet
            Bitboard quiet = targets & ~enemies;
            while (quiet) {
                Square dst_sq = find_lsb_bitboard(quiet);
                moves.add(src_sq, dst_sq, MoveType::Quiet);
                quiet = pop_lsb_bitboard(quiet);
            }

            // Rook captures
            Bitboard captures = targets & enemies;
            while (captures) {
                Square dst_sq = find_lsb_bitboard(captures);
                moves.add(src_sq, dst_sq, MoveType::Capture);
                captures = pop_lsb_bitboard(captures);
            }

            rooks_unpinned = pop_lsb_bitboard(rooks_unpinned);
        }
    }

    void MoveGen::generate_bishop_moves(MoveList &moves) {
        // Pinned bishops
        Bitboard bishops_pinned = f_bishop & pinmask;
        while (bishops_pinned) {
            Square src_sq = find_lsb_bitboard(bishops_pinned);
            Bitboard targets =
                bishop_attacks(src_sq, friends, enemies) & checkmask & pinmask;

            // Bishop quiet
            Bitboard quiet = targets & ~enemies;
            while (quiet) {
                Square dst_sq = find_lsb_bitboard(quiet);
                moves.add(src_sq, dst_sq, MoveType::Quiet);
                quiet = pop_lsb_bitboard(quiet);
            }

            // Bishop captures
            Bitboard captures = targets & enemies;
            while (captures) {
                Square dst_sq = find_lsb_bitboard(captures);
                moves.add(src_sq, dst_sq, MoveType::Capture);
                captures = pop_lsb_bitboard(captures);
            }

            bishops_pinned = pop_lsb_bitboard(bishops_pinned);
        }

        // Unpinned bishops
        Bitboard bishops_unpinned = f_bishop & ~pinmask;
        while (bishops_unpinned) {
            Square src_sq = find_lsb_bitboard(bishops_unpinned);
            Bitboard targets =
                bishop_attacks(src_sq, friends, enemies) & checkmask;

            // Bishop quiet
            Bitboard quiet = targets & ~enemies;
            while (quiet) {
                Square dst_sq = find_lsb_bitboard(quiet);
                moves.add(src_sq, dst_sq, MoveType::Quiet);
                quiet = pop_lsb_bitboard(quiet);
            }

            // Bishop captures
            Bitboard captures = targets & enemies;
            while (captures) {
                Square dst_sq = find_lsb_bitboard(captures);
                moves.add(src_sq, dst_sq, MoveType::Capture);
                captures = pop_lsb_bitboard(captures);
            }

            bishops_unpinned = pop_lsb_bitboard(bishops_unpinned);
        }
    }

    void MoveGen::generate_queen_moves(MoveList &moves) {
        // Pinned queens
        Bitboard queens_pinned = f_queen & pinmask;
        while (queens_pinned) {
            Square src_sq = find_lsb_bitboard(queens_pinned);
            Bitboard targets =
                queen_attacks(src_sq, friends, enemies) & checkmask & pinmask;

            // Queen quiet
            Bitboard quiet = targets & ~enemies;
            while (quiet) {
                Square dst_sq = find_lsb_bitboard(quiet);
                moves.add(src_sq, dst_sq, MoveType::Quiet);
                quiet = pop_lsb_bitboard(quiet);
            }

            // Queen captures
            Bitboard captures = targets & enemies;
            while (captures) {
                Square dst_sq = find_lsb_bitboard(captures);
                moves.add(src_sq, dst_sq, MoveType::Capture);
                captures = pop_lsb_bitboard(captures);
            }

            queens_pinned = pop_lsb_bitboard(queens_pinned);
        }

        // Unpinned queens
        Bitboard queens_unpinned = f_queen & ~pinmask;
        while (queens_unpinned) {
            Square src_sq = find_lsb_bitboard(queens_unpinned);
            Bitboard targets =
                queen_attacks(src_sq, friends, enemies) & checkmask;

            // Queen quiet
            Bitboard quiet = targets & ~enemies;
            while (quiet) {
                Square dst_sq = find_lsb_bitboard(quiet);
                moves.add(src_sq, dst_sq, MoveType::Quiet);
                quiet = pop_lsb_bitboard(quiet);
            }

            // Queen captures
            Bitboard captures = targets & enemies;
            while (captures) {
                Square dst_sq = find_lsb_bitboard(captures);
                moves.add(src_sq, dst_sq, MoveType::Capture);
                captures = pop_lsb_bitboard(captures);
            }

            queens_unpinned = pop_lsb_bitboard(queens_unpinned);
        }
    }

    bool MoveGen::generate(MoveList &moves) {
        compute_attackmask();
        compute_checkmask();
        compute_pinmasks();

        // Only generate the remaining moves if not double-checked
        generate_king_moves(moves);
        if (!check || count_set_bitboard(checkmask & enemies) < 2) {
            generate_pawn_moves(moves);
            generate_rook_moves(moves);
            generate_knight_moves(moves);
            generate_bishop_moves(moves);
            generate_queen_moves(moves);
        }
        return check;
    }
} // namespace Brainiac