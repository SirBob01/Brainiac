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

        Bitboard pinmask_n = SQUARE_NORTH_RAY[sq] & king_slider;
        Bitboard pinmask_s = SQUARE_SOUTH_RAY[sq] & king_slider;
        Bitboard pinmask_e = SQUARE_EAST_RAY[sq] & king_slider;
        Bitboard pinmask_w = SQUARE_WEST_RAY[sq] & king_slider;

        Bitboard pinmask_ne = SQUARE_NORTHEAST_RAY[sq] & king_slider;
        Bitboard pinmask_nw = SQUARE_NORTHWEST_RAY[sq] & king_slider;
        Bitboard pinmask_se = SQUARE_SOUTHEAST_RAY[sq] & king_slider;
        Bitboard pinmask_sw = SQUARE_SOUTHWEST_RAY[sq] & king_slider;

        Bitboard o_hv = o_rook | o_queen;
        Bitboard o_d12 = o_bishop | o_queen;

        bool m_n = (pinmask_n & o_hv) && !pop_lsb_bitboard(pinmask_n & friends);
        bool m_s = (pinmask_s & o_hv) && !pop_lsb_bitboard(pinmask_s & friends);
        bool m_e = (pinmask_e & o_hv) && !pop_lsb_bitboard(pinmask_e & friends);
        bool m_w = (pinmask_w & o_hv) && !pop_lsb_bitboard(pinmask_w & friends);
        pinmask_n &= -m_n;
        pinmask_s &= -m_s;
        pinmask_e &= -m_e;
        pinmask_w &= -m_w;

        bool m_ne =
            (pinmask_ne & o_d12) && !pop_lsb_bitboard(pinmask_ne & friends);
        bool m_nw =
            (pinmask_nw & o_d12) && !pop_lsb_bitboard(pinmask_nw & friends);
        bool m_se =
            (pinmask_se & o_d12) && !pop_lsb_bitboard(pinmask_se & friends);
        bool m_sw =
            (pinmask_sw & o_d12) && !pop_lsb_bitboard(pinmask_sw & friends);
        pinmask_ne &= -m_ne;
        pinmask_nw &= -m_nw;
        pinmask_se &= -m_se;
        pinmask_sw &= -m_sw;

        pinmask_hv = pinmask_e | pinmask_w | pinmask_n | pinmask_s;
        pinmask_d12 = pinmask_ne | pinmask_sw | pinmask_nw | pinmask_se;
        pinmask = pinmask_hv | pinmask_d12;
    }

    void MoveGen::generate_king_moves(MoveList &moves) {
        Bitboard danger = o_pawn_attacks | o_knight_attacks | o_king_attacks;
        Bitboard no_king = all & ~f_king;

        Bitboard bishops = o_bishop;
        while (bishops) {
            Square sq = find_lsb_bitboard(bishops);
            danger |= bishop_attacks(sq, 0, no_king);
            bishops = pop_lsb_bitboard(bishops);
        }

        Bitboard rooks = o_rook;
        while (rooks) {
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
            Square dst_sq = find_lsb_bitboard(captures);
            moves.add(src_sq, dst_sq, MoveType::Capture);
            captures = pop_lsb_bitboard(captures);
        }

        // King castling
        if (!check) {
            CastlingRight king_side = static_cast<CastlingRight>(2 * turn);
            if (castling & (1 << king_side)) {
                Bitboard king_pass = CASTLING_MASKS[king_side];
                if (!(king_pass & attackmask) && !(king_pass & all)) {
                    Square dst_sq = static_cast<Square>(src_sq + 2);
                    moves.add(src_sq, dst_sq, MoveType::KingCastle);
                }
            }

            CastlingRight queen_side = static_cast<CastlingRight>(2 * turn + 1);
            if ((castling & (1 << queen_side))) {
                Bitboard queen_pass = CASTLING_MASKS[queen_side];
                if (!(queen_pass & ~FILES[1] & attackmask) &&
                    !(queen_pass & all)) {
                    Square dst_sq = static_cast<Square>(src_sq - 2);
                    moves.add(src_sq, dst_sq, MoveType::QueenCastle);
                }
            }
        }
    }

    void MoveGen::generate_pawn_moves(MoveList &moves) {
        Square king_sq = find_lsb_bitboard(f_king);
        Bitboard ep_target = SQUARES[ep_dst];
        Bitboard ep_capture = (ep_target << 8) >> (!turn << 4);
        Bitboard ep_condition = ep_capture & checkmask;
        Bitboard horizon_mask = (o_queen | o_rook) & SQUARE_RANKS[king_sq];

        // Unpinned pawns
        Bitboard pawns = f_pawn & ~pinmask;
        while (pawns) {
            Square src_sq = find_lsb_bitboard(pawns);
            Bitboard advances = pawn_advances(src_sq, turn) & ~all;
            Bitboard doubles = pawn_doubles(src_sq, turn) & ~all;
            Bitboard captures = pawn_captures(src_sq, turn);

            // Pawn advances with promotions
            Bitboard advances_checkmask = advances & checkmask;
            Bitboard advances_only = advances_checkmask & ~PROMOTION_MASK;
            Bitboard advances_promote = advances_checkmask & PROMOTION_MASK;
            while (advances_only) {
                Square dst_sq = find_lsb_bitboard(advances_only);
                moves.add(src_sq, dst_sq, MoveType::Quiet);
                advances_only = pop_lsb_bitboard(advances_only);
            }
            while (advances_promote) {
                Square dst_sq = find_lsb_bitboard(advances_promote);
                moves.add(src_sq, dst_sq, MoveType::KnightPromo);
                moves.add(src_sq, dst_sq, MoveType::RookPromo);
                moves.add(src_sq, dst_sq, MoveType::BishopPromo);
                moves.add(src_sq, dst_sq, MoveType::QueenPromo);
                advances_promote = pop_lsb_bitboard(advances_promote);
            }

            // Pawn double advance
            Bitboard double_mask = -static_cast<bool>(advances) & checkmask;
            Bitboard doubles_only = doubles & double_mask;
            while (doubles_only) {
                Square dst_sq = find_lsb_bitboard(doubles_only);
                moves.add(src_sq, dst_sq, MoveType::PawnDouble);
                doubles_only = pop_lsb_bitboard(doubles_only);
            }

            // Pawn captures with promotions and en-passant
            Bitboard captures_checkmask = captures & enemies & checkmask;
            Bitboard captures_only = captures_checkmask & ~PROMOTION_MASK;
            Bitboard captures_promote = captures_checkmask & PROMOTION_MASK;
            Bitboard captures_ep = captures & ep_target;
            while (captures_only) {
                Square dst_sq = find_lsb_bitboard(captures_only);
                moves.add(src_sq, dst_sq, MoveType::Capture);
                captures_only = pop_lsb_bitboard(captures_only);
            }
            while (captures_promote) {
                Square dst_sq = find_lsb_bitboard(captures_promote);
                moves.add(src_sq, dst_sq, MoveType::KnightPromoCapture);
                moves.add(src_sq, dst_sq, MoveType::RookPromoCapture);
                moves.add(src_sq, dst_sq, MoveType::BishopPromoCapture);
                moves.add(src_sq, dst_sq, MoveType::QueenPromoCapture);
                captures_promote = pop_lsb_bitboard(captures_promote);
            }
            if (captures_ep && ep_condition) {
                Bitboard horizon_check =
                    rook_attacks(king_sq,
                                 friends & ~(1ULL << src_sq),
                                 enemies & ~ep_capture) &
                    horizon_mask;
                if (!horizon_check) {
                    Square dst_sq = find_lsb_bitboard(captures_ep);
                    moves.add(src_sq, dst_sq, MoveType::EnPassant);
                }
            }

            pawns = pop_lsb_bitboard(pawns);
        }

        // Pinned pawns HV
        Bitboard check_pinned_hv = pinmask_hv & checkmask;
        Bitboard pawns_pinned_hv = f_pawn & pinmask_hv;
        while (pawns_pinned_hv) {
            Square src_sq = find_lsb_bitboard(pawns_pinned_hv);
            Bitboard advances = pawn_advances(src_sq, turn) & ~all;
            Bitboard doubles = pawn_doubles(src_sq, turn) & ~all;

            // Pawn advances with promotions
            Bitboard advances_checkmask = advances & check_pinned_hv;
            Bitboard advances_only = advances_checkmask & ~PROMOTION_MASK;
            Bitboard advances_promote = advances_checkmask & PROMOTION_MASK;
            while (advances_only) {
                Square dst_sq = find_lsb_bitboard(advances_only);
                moves.add(src_sq, dst_sq, MoveType::Quiet);
                advances_only = pop_lsb_bitboard(advances_only);
            }
            while (advances_promote) {
                Square dst_sq = find_lsb_bitboard(advances_promote);
                moves.add(src_sq, dst_sq, MoveType::KnightPromo);
                moves.add(src_sq, dst_sq, MoveType::RookPromo);
                moves.add(src_sq, dst_sq, MoveType::BishopPromo);
                moves.add(src_sq, dst_sq, MoveType::QueenPromo);
                advances_promote = pop_lsb_bitboard(advances_promote);
            }

            // Pawn double advance
            Bitboard double_mask =
                -static_cast<bool>(advances) & check_pinned_hv;
            Bitboard doubles_only = doubles & double_mask;
            while (doubles_only) {
                Square dst_sq = find_lsb_bitboard(doubles_only);
                moves.add(src_sq, dst_sq, MoveType::PawnDouble);
                doubles_only = pop_lsb_bitboard(doubles_only);
            }

            pawns_pinned_hv = pop_lsb_bitboard(pawns_pinned_hv);
        }

        // Pinned pawns D12
        Bitboard pawns_pinned_d12 = f_pawn & pinmask_d12;
        while (pawns_pinned_d12) {
            Square src_sq = find_lsb_bitboard(pawns_pinned_d12);
            Bitboard captures = pawn_captures(src_sq, turn);

            // Pawn captures with promotions and en-passant
            Bitboard captures_checkmask =
                captures & enemies & pinmask_d12 & checkmask;
            Bitboard captures_only = captures_checkmask & ~PROMOTION_MASK;
            Bitboard captures_promote = captures_checkmask & PROMOTION_MASK;
            Bitboard captures_ep = captures & ep_target & pinmask_d12;
            while (captures_only) {
                Square dst_sq = find_lsb_bitboard(captures_only);
                moves.add(src_sq, dst_sq, MoveType::Capture);
                captures_only = pop_lsb_bitboard(captures_only);
            }
            while (captures_promote) {
                Square dst_sq = find_lsb_bitboard(captures_promote);
                moves.add(src_sq, dst_sq, MoveType::KnightPromoCapture);
                moves.add(src_sq, dst_sq, MoveType::RookPromoCapture);
                moves.add(src_sq, dst_sq, MoveType::BishopPromoCapture);
                moves.add(src_sq, dst_sq, MoveType::QueenPromoCapture);
                captures_promote = pop_lsb_bitboard(captures_promote);
            }
            if (captures_ep && ep_condition) {
                Bitboard horizon_check =
                    rook_attacks(king_sq,
                                 friends & ~(1ULL << src_sq),
                                 enemies & ~ep_capture) &
                    horizon_mask;
                if (!horizon_check) {
                    Square dst_sq = find_lsb_bitboard(captures_ep);
                    moves.add(src_sq, dst_sq, MoveType::EnPassant);
                }
            }

            pawns_pinned_d12 = pop_lsb_bitboard(pawns_pinned_d12);
        }
    }

    void MoveGen::generate_knight_moves(MoveList &moves) {
        Bitboard knights = f_knight & ~pinmask;
        Bitboard target_mask = ~friends & checkmask;
        while (knights) {
            Square src_sq = find_lsb_bitboard(knights);
            Bitboard targets = knight_attacks(src_sq) & target_mask;

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
        // Unpinned rooks
        Bitboard rooks = f_rook & ~pinmask;
        while (rooks) {
            Square src_sq = find_lsb_bitboard(rooks);
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

            rooks = pop_lsb_bitboard(rooks);
        }

        // Pinned rooks
        Bitboard rooks_pinned = f_rook & pinmask_hv;
        while (rooks_pinned) {
            Square src_sq = find_lsb_bitboard(rooks_pinned);
            Bitboard targets =
                rook_attacks(src_sq, friends, enemies) & checkmask & pinmask_hv;

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
    }

    void MoveGen::generate_bishop_moves(MoveList &moves) {
        // Unpinned bishops
        Bitboard bishops = f_bishop & ~pinmask;
        while (bishops) {
            Square src_sq = find_lsb_bitboard(bishops);
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

            bishops = pop_lsb_bitboard(bishops);
        }

        // Pinned bishops
        Bitboard bishops_pinned = f_bishop & pinmask_d12;
        while (bishops_pinned) {
            Square src_sq = find_lsb_bitboard(bishops_pinned);
            Bitboard targets = bishop_attacks(src_sq, friends, enemies) &
                               checkmask & pinmask_d12;

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
    }

    void MoveGen::generate_queen_moves(MoveList &moves) {
        // Unpinned queens
        Bitboard queens = f_queen & ~pinmask;
        while (queens) {
            Square src_sq = find_lsb_bitboard(queens);
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

            queens = pop_lsb_bitboard(queens);
        }

        // Pinned queens HV
        Bitboard queens_pinned_hv = f_queen & pinmask_hv;
        while (queens_pinned_hv) {
            Square src_sq = find_lsb_bitboard(queens_pinned_hv);
            Bitboard targets =
                rook_attacks(src_sq, friends, enemies) & checkmask & pinmask_hv;

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

            queens_pinned_hv = pop_lsb_bitboard(queens_pinned_hv);
        }

        // Pinned queens D12
        Bitboard queens_pinned_d12 = f_queen & pinmask_d12;
        while (queens_pinned_d12) {
            Square src_sq = find_lsb_bitboard(queens_pinned_d12);
            Bitboard targets = bishop_attacks(src_sq, friends, enemies) &
                               checkmask & pinmask_d12;

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

            queens_pinned_d12 = pop_lsb_bitboard(queens_pinned_d12);
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