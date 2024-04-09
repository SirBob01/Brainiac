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

    Bitboard MoveGen::compute_attackmask() {
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

        return (o_king_attacks | o_pawn_attacks | o_knight_attacks |
                o_rook_h_attacks | o_rook_v_attacks | o_bishop_d1_attacks |
                o_bishop_d2_attacks | o_queen_h_attacks | o_queen_v_attacks |
                o_queen_d1_attacks | o_queen_d2_attacks) &
               ~enemies;
    }

    Bitboard MoveGen::compute_checkmask() {
        Square sq = find_lsb_bitboard(f_king);
        Bitboard king_horizontal = SQUARE_RANKS[sq];
        Bitboard king_vertical = SQUARE_FILES[sq];
        Bitboard king_diagonal = SQUARE_DIAGONALS[sq];
        Bitboard king_anti_diagonal = SQUARE_ANTI_DIAGONALS[sq];

        Bitboard king_hv_slider = rook_attacks(sq, friends, enemies);
        Bitboard king_d12_slider = bishop_attacks(sq, friends, enemies);

        Bitboard h_mask =
            o_rook | o_rook_h_attacks | o_queen | o_queen_h_attacks;
        Bitboard v_mask =
            o_rook | o_rook_v_attacks | o_queen | o_queen_v_attacks;
        Bitboard d1_mask =
            o_bishop | o_bishop_d1_attacks | o_queen | o_queen_d1_attacks;
        Bitboard d2_mask =
            o_bishop | o_bishop_d2_attacks | o_queen | o_queen_d2_attacks;
        Bitboard mask =
            // Pawn
            (pawn_captures(sq, turn) & o_pawn) |
            // Knight
            (knight_attacks(sq) & o_knight) |
            // HV sliders
            (king_hv_slider & king_horizontal & h_mask) |
            (king_hv_slider & king_vertical & v_mask) |
            // D12 sliders
            (king_d12_slider & king_diagonal & d1_mask) |
            (king_d12_slider & king_anti_diagonal & d2_mask);
        return mask;
    }

    void MoveGen::generate_king_moves(MoveList &moves, Bitboard checkmask) {}
    void MoveGen::generate_pawn_moves(MoveList &moves, Bitboard checkmask) {}
    void MoveGen::generate_rook_moves(MoveList &moves, Bitboard checkmask) {}
    void MoveGen::generate_knight_moves(MoveList &moves, Bitboard checkmask) {}
    void MoveGen::generate_bishop_moves(MoveList &moves, Bitboard checkmask) {}
    void MoveGen::generate_queen_moves(MoveList &moves, Bitboard checkmask) {}

    bool MoveGen::generate(MoveList &moves) {
        Bitboard attackmask = compute_attackmask();
        bool check = attackmask & f_king;
        Bitboard checkmask = check ? compute_checkmask() : -1;

        generate_king_moves(moves, checkmask);
        generate_pawn_moves(moves, checkmask);
        generate_rook_moves(moves, checkmask);
        generate_knight_moves(moves, checkmask);
        generate_bishop_moves(moves, checkmask);
        generate_queen_moves(moves, checkmask);

        return check;
    }
} // namespace Brainiac