#pragma once

#include <array>

#include "Bitboard.hpp"
#include "Move.hpp"
#include "MoveList.hpp"
#include "Sliders.hpp"

namespace Brainiac {
    /**
     * @brief Table of king moves by bit index.
     *
     */
    constexpr std::array<Bitboard, 64> KING_MOVE_MASKS = {
        0x0000000000000302, 0x0000000000000705, 0x0000000000000e0a,
        0x0000000000001c14, 0x0000000000003828, 0x0000000000007050,
        0x000000000000e0a0, 0x000000000000c040, 0x0000000000030203,
        0x0000000000070507, 0x00000000000e0a0e, 0x00000000001c141c,
        0x0000000000382838, 0x0000000000705070, 0x0000000000e0a0e0,
        0x0000000000c040c0, 0x0000000003020300, 0x0000000007050700,
        0x000000000e0a0e00, 0x000000001c141c00, 0x0000000038283800,
        0x0000000070507000, 0x00000000e0a0e000, 0x00000000c040c000,
        0x0000000302030000, 0x0000000705070000, 0x0000000e0a0e0000,
        0x0000001c141c0000, 0x0000003828380000, 0x0000007050700000,
        0x000000e0a0e00000, 0x000000c040c00000, 0x0000030203000000,
        0x0000070507000000, 0x00000e0a0e000000, 0x00001c141c000000,
        0x0000382838000000, 0x0000705070000000, 0x0000e0a0e0000000,
        0x0000c040c0000000, 0x0003020300000000, 0x0007050700000000,
        0x000e0a0e00000000, 0x001c141c00000000, 0x0038283800000000,
        0x0070507000000000, 0x00e0a0e000000000, 0x00c040c000000000,
        0x0302030000000000, 0x0705070000000000, 0x0e0a0e0000000000,
        0x1c141c0000000000, 0x3828380000000000, 0x7050700000000000,
        0xe0a0e00000000000, 0xc040c00000000000, 0x0203000000000000,
        0x0507000000000000, 0x0a0e000000000000, 0x141c000000000000,
        0x2838000000000000, 0x5070000000000000, 0xa0e0000000000000,
        0x40c0000000000000,
    };

    /**
     * @brief Table of knight moves by bit index.
     *
     */
    constexpr std::array<Bitboard, 64> KNIGHT_MOVE_MASKS = {
        0x0000000000020400, 0x0000000000050800, 0x00000000000a1100,
        0x0000000000142200, 0x0000000000284400, 0x0000000000508800,
        0x0000000000a01000, 0x0000000000402000, 0x0000000002040004,
        0x0000000005080008, 0x000000000a110011, 0x0000000014220022,
        0x0000000028440044, 0x0000000050880088, 0x00000000a0100010,
        0x0000000040200020, 0x0000000204000402, 0x0000000508000805,
        0x0000000a1100110a, 0x0000001422002214, 0x0000002844004428,
        0x0000005088008850, 0x000000a0100010a0, 0x0000004020002040,
        0x0000020400040200, 0x0000050800080500, 0x00000a1100110a00,
        0x0000142200221400, 0x0000284400442800, 0x0000508800885000,
        0x0000a0100010a000, 0x0000402000204000, 0x0002040004020000,
        0x0005080008050000, 0x000a1100110a0000, 0x0014220022140000,
        0x0028440044280000, 0x0050880088500000, 0x00a0100010a00000,
        0x0040200020400000, 0x0204000402000000, 0x0508000805000000,
        0x0a1100110a000000, 0x1422002214000000, 0x2844004428000000,
        0x5088008850000000, 0xa0100010a0000000, 0x4020002040000000,
        0x0400040200000000, 0x0800080500000000, 0x1100110a00000000,
        0x2200221400000000, 0x4400442800000000, 0x8800885000000000,
        0x100010a000000000, 0x2000204000000000, 0x0004020000000000,
        0x0008050000000000, 0x00110a0000000000, 0x0022140000000000,
        0x0044280000000000, 0x0088500000000000, 0x0010a00000000000,
        0x0020400000000000,
    };

    /**
     * @brief Pawn advance masks. The first 64 entries are for white, remaining
     * 64 are for black.
     *
     */
    constexpr std::array<Bitboard, 128> PAWN_ADVANCE_MASKS = {
        0x0000000000000100, 0x0000000000000200, 0x0000000000000400,
        0x0000000000000800, 0x0000000000001000, 0x0000000000002000,
        0x0000000000004000, 0x0000000000008000, 0x0000000000010000,
        0x0000000000020000, 0x0000000000040000, 0x0000000000080000,
        0x0000000000100000, 0x0000000000200000, 0x0000000000400000,
        0x0000000000800000, 0x0000000001000000, 0x0000000002000000,
        0x0000000004000000, 0x0000000008000000, 0x0000000010000000,
        0x0000000020000000, 0x0000000040000000, 0x0000000080000000,
        0x0000000100000000, 0x0000000200000000, 0x0000000400000000,
        0x0000000800000000, 0x0000001000000000, 0x0000002000000000,
        0x0000004000000000, 0x0000008000000000, 0x0000010000000000,
        0x0000020000000000, 0x0000040000000000, 0x0000080000000000,
        0x0000100000000000, 0x0000200000000000, 0x0000400000000000,
        0x0000800000000000, 0x0001000000000000, 0x0002000000000000,
        0x0004000000000000, 0x0008000000000000, 0x0010000000000000,
        0x0020000000000000, 0x0040000000000000, 0x0080000000000000,
        0x0100000000000000, 0x0200000000000000, 0x0400000000000000,
        0x0800000000000000, 0x1000000000000000, 0x2000000000000000,
        0x4000000000000000, 0x8000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000,

        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000001,
        0x0000000000000002, 0x0000000000000004, 0x0000000000000008,
        0x0000000000000010, 0x0000000000000020, 0x0000000000000040,
        0x0000000000000080, 0x0000000000000100, 0x0000000000000200,
        0x0000000000000400, 0x0000000000000800, 0x0000000000001000,
        0x0000000000002000, 0x0000000000004000, 0x0000000000008000,
        0x0000000000010000, 0x0000000000020000, 0x0000000000040000,
        0x0000000000080000, 0x0000000000100000, 0x0000000000200000,
        0x0000000000400000, 0x0000000000800000, 0x0000000001000000,
        0x0000000002000000, 0x0000000004000000, 0x0000000008000000,
        0x0000000010000000, 0x0000000020000000, 0x0000000040000000,
        0x0000000080000000, 0x0000000100000000, 0x0000000200000000,
        0x0000000400000000, 0x0000000800000000, 0x0000001000000000,
        0x0000002000000000, 0x0000004000000000, 0x0000008000000000,
        0x0000010000000000, 0x0000020000000000, 0x0000040000000000,
        0x0000080000000000, 0x0000100000000000, 0x0000200000000000,
        0x0000400000000000, 0x0000800000000000, 0x0001000000000000,
        0x0002000000000000, 0x0004000000000000, 0x0008000000000000,
        0x0010000000000000, 0x0020000000000000, 0x0040000000000000,
        0x0080000000000000,
    };

    constexpr std::array<Bitboard, 128> PAWN_DOUBLE_MASKS = {
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000001000000,
        0x0000000002000000, 0x0000000004000000, 0x0000000008000000,
        0x0000000010000000, 0x0000000020000000, 0x0000000040000000,
        0x0000000080000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000,

        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000100000000, 0x0000000200000000, 0x0000000400000000,
        0x0000000800000000, 0x0000001000000000, 0x0000002000000000,
        0x0000004000000000, 0x0000008000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000,
    };

    /**
     * @brief Pawn capture masks. The first 64 entries are for white, remaining
     * 64 are for black.
     *
     */
    constexpr std::array<Bitboard, 128> PAWN_CAPTURE_MASKS = {
        0x0000000000000200, 0x0000000000000500, 0x0000000000000a00,
        0x0000000000001400, 0x0000000000002800, 0x0000000000005000,
        0x000000000000a000, 0x0000000000004000, 0x0000000000020000,
        0x0000000000050000, 0x00000000000a0000, 0x0000000000140000,
        0x0000000000280000, 0x0000000000500000, 0x0000000000a00000,
        0x0000000000400000, 0x0000000002000000, 0x0000000005000000,
        0x000000000a000000, 0x0000000014000000, 0x0000000028000000,
        0x0000000050000000, 0x00000000a0000000, 0x0000000040000000,
        0x0000000200000000, 0x0000000500000000, 0x0000000a00000000,
        0x0000001400000000, 0x0000002800000000, 0x0000005000000000,
        0x000000a000000000, 0x0000004000000000, 0x0000020000000000,
        0x0000050000000000, 0x00000a0000000000, 0x0000140000000000,
        0x0000280000000000, 0x0000500000000000, 0x0000a00000000000,
        0x0000400000000000, 0x0002000000000000, 0x0005000000000000,
        0x000a000000000000, 0x0014000000000000, 0x0028000000000000,
        0x0050000000000000, 0x00a0000000000000, 0x0040000000000000,
        0x0200000000000000, 0x0500000000000000, 0x0a00000000000000,
        0x1400000000000000, 0x2800000000000000, 0x5000000000000000,
        0xa000000000000000, 0x4000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000,

        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
        0x0000000000000000, 0x0000000000000000, 0x0000000000000002,
        0x0000000000000005, 0x000000000000000a, 0x0000000000000014,
        0x0000000000000028, 0x0000000000000050, 0x00000000000000a0,
        0x0000000000000040, 0x0000000000000200, 0x0000000000000500,
        0x0000000000000a00, 0x0000000000001400, 0x0000000000002800,
        0x0000000000005000, 0x000000000000a000, 0x0000000000004000,
        0x0000000000020000, 0x0000000000050000, 0x00000000000a0000,
        0x0000000000140000, 0x0000000000280000, 0x0000000000500000,
        0x0000000000a00000, 0x0000000000400000, 0x0000000002000000,
        0x0000000005000000, 0x000000000a000000, 0x0000000014000000,
        0x0000000028000000, 0x0000000050000000, 0x00000000a0000000,
        0x0000000040000000, 0x0000000200000000, 0x0000000500000000,
        0x0000000a00000000, 0x0000001400000000, 0x0000002800000000,
        0x0000005000000000, 0x000000a000000000, 0x0000004000000000,
        0x0000020000000000, 0x0000050000000000, 0x00000a0000000000,
        0x0000140000000000, 0x0000280000000000, 0x0000500000000000,
        0x0000a00000000000, 0x0000400000000000, 0x0002000000000000,
        0x0005000000000000, 0x000a000000000000, 0x0014000000000000,
        0x0028000000000000, 0x0050000000000000, 0x00a0000000000000,
        0x0040000000000000,
    };

    /**
     * @brief Compute king attack mask.
     *
     * @param sq
     * @return Bitboard
     */
    Bitboard king_attacks(Square sq);

    /**
     * @brief Compute pawn advance mask.
     *
     * @param sq
     * @param turn
     * @return Bitboard
     */
    Bitboard pawn_advances(Square sq, Color turn);

    /**
     * @brief Compute pawn double mask.
     *
     * @param sq
     * @param turn
     * @return Bitboard
     */
    Bitboard pawn_doubles(Square sq, Color turn);

    /**
     * @brief Compute pawn capture mask.
     *
     * @param sq
     * @param turn
     * @return Bitboard
     */
    Bitboard pawn_captures(Square sq, Color turn);

    /**
     * @brief Compute knight attack mask.
     *
     * @param sq
     * @return Bitboard
     */
    Bitboard knight_attacks(Square sq);

    /**
     * @brief Compute rook attack mask.
     *
     * @param sq
     * @param friends
     * @param enemies
     * @return Bitboard
     */
    Bitboard rook_attacks(Square sq, Bitboard friends, Bitboard enemies);

    /**
     * @brief Compute bishop attack mask.
     *
     * @param sq
     * @param friends
     * @param enemies
     * @return Bitboard
     */
    Bitboard bishop_attacks(Square sq, Bitboard friends, Bitboard enemies);

    /**
     * @brief Compute queen attack mask.
     *
     * @param sq
     * @param friends
     * @param enemies
     * @return Bitboard
     */
    Bitboard queen_attacks(Square sq, Bitboard friends, Bitboard enemies);

    struct MoveGen {
        Bitboard friends;
        Bitboard enemies;
        Bitboard all;

        Bitboard f_king;
        Bitboard f_pawn;
        Bitboard f_rook;
        Bitboard f_knight;
        Bitboard f_bishop;
        Bitboard f_queen;

        Bitboard o_king;
        Bitboard o_pawn;
        Bitboard o_rook;
        Bitboard o_knight;
        Bitboard o_bishop;
        Bitboard o_queen;

        Bitboard ep;
        Color turn;

        /**
         * @brief Generate the moves and add them to the move list. Returns true
         * if king is check.
         *
         * @param moves
         * @return true
         * @return false
         */
        bool generate(MoveList &moves);

      private:
        Bitboard o_king_attacks;
        Bitboard o_pawn_attacks;
        Bitboard o_knight_attacks;

        Bitboard o_rook_h_attacks;
        Bitboard o_rook_v_attacks;

        Bitboard o_bishop_d1_attacks;
        Bitboard o_bishop_d2_attacks;

        Bitboard o_queen_h_attacks;
        Bitboard o_queen_v_attacks;
        Bitboard o_queen_d1_attacks;
        Bitboard o_queen_d2_attacks;

        Bitboard attackmask;
        Bitboard checkmask;
        Bitboard pinmask_h;
        Bitboard pinmask_v;
        Bitboard pinmask_d1;
        Bitboard pinmask_d2;
        Bitboard pinmask;

        /**
         * @brief Compute the attackmask of the opponent.
         *
         */
        void compute_attackmask();

        /**
         * @brief Compute the pinmask.
         *
         */
        void compute_pinmasks();

        /**
         * @brief Compute the checkmask, which is the path from any opponent
         * piece to the friendly king.
         *
         * @param check
         */
        void compute_checkmask(bool check);

        /**
         * @brief Generate king moves.
         *
         * @param moves
         */
        void generate_king_moves(MoveList &moves);

        /**
         * @brief Generate pawn moves.
         *
         * @param moves
         */
        void generate_pawn_moves(MoveList &moves);

        /**
         * @brief Generate rook moves.
         *
         * @param moves
         */
        void generate_rook_moves(MoveList &moves);

        /**
         * @brief Generate knight moves.
         *
         * @param moves
         */
        void generate_knight_moves(MoveList &moves);

        /**
         * @brief Generate bishop moves.
         *
         * @param moves
         */
        void generate_bishop_moves(MoveList &moves);

        /**
         * @brief Generate queen moves.
         *
         * @param moves
         */
        void generate_queen_moves(MoveList &moves);
    };
} // namespace Brainiac