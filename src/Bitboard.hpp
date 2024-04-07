#pragma once

#include <array>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <stack>

namespace Brainiac {
    /**
     * @brief Represent the 8x8 board occupancy as a 64 bit integer.
     *
     */
    using Bitboard = uint64_t;

    /**
     * @brief Rank masks.
     *
     */
    constexpr std::array<Bitboard, 8> RANKS = {
        0x00000000000000ff,
        0x000000000000ff00,
        0x0000000000ff0000,
        0x00000000ff000000,
        0x000000ff00000000,
        0x0000ff0000000000,
        0x00ff000000000000,
        0xff00000000000000,
    };

    /**
     * @brief File masks.
     *
     */
    constexpr std::array<Bitboard, 8> FILES = {
        0x0101010101010101,
        0x0202020202020202,
        0x0404040404040404,
        0x0808080808080808,
        0x1010101010101010,
        0x2020202020202020,
        0x4040404040404040,
        0x8080808080808080,
    };

    constexpr std::array<Bitboard, 64> SQUARE_RANKS = {
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff00,
        0xff00,
        0xff00,
        0xff00,
        0xff00,
        0xff00,
        0xff00,
        0xff00,
        0xff0000,
        0xff0000,
        0xff0000,
        0xff0000,
        0xff0000,
        0xff0000,
        0xff0000,
        0xff0000,
        0xff000000,
        0xff000000,
        0xff000000,
        0xff000000,
        0xff000000,
        0xff000000,
        0xff000000,
        0xff000000,
        0xff00000000,
        0xff00000000,
        0xff00000000,
        0xff00000000,
        0xff00000000,
        0xff00000000,
        0xff00000000,
        0xff00000000,
        0xff0000000000,
        0xff0000000000,
        0xff0000000000,
        0xff0000000000,
        0xff0000000000,
        0xff0000000000,
        0xff0000000000,
        0xff0000000000,
        0xff000000000000,
        0xff000000000000,
        0xff000000000000,
        0xff000000000000,
        0xff000000000000,
        0xff000000000000,
        0xff000000000000,
        0xff000000000000,
        0xff00000000000000,
        0xff00000000000000,
        0xff00000000000000,
        0xff00000000000000,
        0xff00000000000000,
        0xff00000000000000,
        0xff00000000000000,
        0xff00000000000000,
    };

    constexpr std::array<Bitboard, 64> SQUARE_FILES = {
        0x101010101010101,  0x202020202020202,  0x404040404040404,
        0x808080808080808,  0x1010101010101010, 0x2020202020202020,
        0x4040404040404040, 0x8080808080808080, 0x101010101010101,
        0x202020202020202,  0x404040404040404,  0x808080808080808,
        0x1010101010101010, 0x2020202020202020, 0x4040404040404040,
        0x8080808080808080, 0x101010101010101,  0x202020202020202,
        0x404040404040404,  0x808080808080808,  0x1010101010101010,
        0x2020202020202020, 0x4040404040404040, 0x8080808080808080,
        0x101010101010101,  0x202020202020202,  0x404040404040404,
        0x808080808080808,  0x1010101010101010, 0x2020202020202020,
        0x4040404040404040, 0x8080808080808080, 0x101010101010101,
        0x202020202020202,  0x404040404040404,  0x808080808080808,
        0x1010101010101010, 0x2020202020202020, 0x4040404040404040,
        0x8080808080808080, 0x101010101010101,  0x202020202020202,
        0x404040404040404,  0x808080808080808,  0x1010101010101010,
        0x2020202020202020, 0x4040404040404040, 0x8080808080808080,
        0x101010101010101,  0x202020202020202,  0x404040404040404,
        0x808080808080808,  0x1010101010101010, 0x2020202020202020,
        0x4040404040404040, 0x8080808080808080, 0x101010101010101,
        0x202020202020202,  0x404040404040404,  0x808080808080808,
        0x1010101010101010, 0x2020202020202020, 0x4040404040404040,
        0x8080808080808080,
    };

    /**
     * @brief Per-square diagonal masks.
     *
     */
    constexpr std::array<Bitboard, 64> SQUARE_DIAGONALS = {
        0x8040201008040201,
        0x80402010080402,
        0x804020100804,
        0x8040201008,
        0x80402010,
        0x804020,
        0x8040,
        0x80,
        0x4020100804020100,
        0x8040201008040201,
        0x80402010080402,
        0x804020100804,
        0x8040201008,
        0x80402010,
        0x804020,
        0x8040,
        0x2010080402010000,
        0x4020100804020100,
        0x8040201008040201,
        0x80402010080402,
        0x804020100804,
        0x8040201008,
        0x80402010,
        0x804020,
        0x1008040201000000,
        0x2010080402010000,
        0x4020100804020100,
        0x8040201008040201,
        0x80402010080402,
        0x804020100804,
        0x8040201008,
        0x80402010,
        0x804020100000000,
        0x1008040201000000,
        0x2010080402010000,
        0x4020100804020100,
        0x8040201008040201,
        0x80402010080402,
        0x804020100804,
        0x8040201008,
        0x402010000000000,
        0x804020100000000,
        0x1008040201000000,
        0x2010080402010000,
        0x4020100804020100,
        0x8040201008040201,
        0x80402010080402,
        0x804020100804,
        0x201000000000000,
        0x402010000000000,
        0x804020100000000,
        0x1008040201000000,
        0x2010080402010000,
        0x4020100804020100,
        0x8040201008040201,
        0x80402010080402,
        0x100000000000000,
        0x201000000000000,
        0x402010000000000,
        0x804020100000000,
        0x1008040201000000,
        0x2010080402010000,
        0x4020100804020100,
        0x8040201008040201,
    };

    /**
     * @brief Per-square antidiagonal masks.
     *
     */
    constexpr std::array<Bitboard, 64> SQUARE_ANTI_DIAGONALS = {
        0x1,
        0x102,
        0x10204,
        0x1020408,
        0x102040810,
        0x10204081020,
        0x1020408102040,
        0x102040810204080,
        0x102,
        0x10204,
        0x1020408,
        0x102040810,
        0x10204081020,
        0x1020408102040,
        0x102040810204080,
        0x204081020408000,
        0x10204,
        0x1020408,
        0x102040810,
        0x10204081020,
        0x1020408102040,
        0x102040810204080,
        0x204081020408000,
        0x408102040800000,
        0x1020408,
        0x102040810,
        0x10204081020,
        0x1020408102040,
        0x102040810204080,
        0x204081020408000,
        0x408102040800000,
        0x810204080000000,
        0x102040810,
        0x10204081020,
        0x1020408102040,
        0x102040810204080,
        0x204081020408000,
        0x408102040800000,
        0x810204080000000,
        0x1020408000000000,
        0x10204081020,
        0x1020408102040,
        0x102040810204080,
        0x204081020408000,
        0x408102040800000,
        0x810204080000000,
        0x1020408000000000,
        0x2040800000000000,
        0x1020408102040,
        0x102040810204080,
        0x204081020408000,
        0x408102040800000,
        0x810204080000000,
        0x1020408000000000,
        0x2040800000000000,
        0x4080000000000000,
        0x102040810204080,
        0x204081020408000,
        0x408102040800000,
        0x810204080000000,
        0x1020408000000000,
        0x2040800000000000,
        0x4080000000000000,
        0x8000000000000000,
    };

    /**
     * @brief Find the index of the least significant bit.
     *
     * @param bitboard
     * @return constexpr unsigned
     */
    constexpr unsigned find_lsb_bitboard(Bitboard bitboard) {
        return __builtin_ctzll(bitboard);
    }

    /**
     * @brief Count the number of set bits.
     *
     * @param bitboard
     * @return constexpr unsigned
     */
    constexpr unsigned count_set_bitboard(Bitboard bitboard) {
        return __builtin_popcountll(bitboard);
    }

    /**
     * @brief Flip a bitboard vertically.
     *
     * @param bitboard
     * @return constexpr Bitboard
     */
    constexpr Bitboard flip_vertical_bitboard(Bitboard bitboard) {
        return __builtin_bswap64(bitboard);
    }

    /**
     * @brief Flip a bitboard horizontally.
     *
     * @param bitboard
     * @return constexpr Bitboard
     */
    constexpr Bitboard flip_horizontal_bitboard(Bitboard bitboard) {
        const Bitboard hk_0 = 0x5555555555555555;
        const Bitboard hk_1 = 0x3333333333333333;
        const Bitboard hk_2 = 0x0f0f0f0f0f0f0f0f;
        bitboard = ((bitboard >> 1) & hk_0) + 2 * (bitboard & hk_0);
        bitboard = ((bitboard >> 2) & hk_1) + 4 * (bitboard & hk_1);
        bitboard = ((bitboard >> 4) & hk_2) + 16 * (bitboard & hk_2);
        return bitboard;
    }

    /**
     * @brief Get the least significant bit.
     *
     * @param bitboard
     * @return constexpr Bitboard
     */
    constexpr Bitboard get_lsb_bitboard(Bitboard bitboard) {
        return bitboard & (-bitboard);
    }

    /**
     * @brief Pop the least significant bit.
     *
     * @param bitboard
     * @return constexpr Bitboard
     */
    constexpr Bitboard pop_lsb_bitboard(Bitboard bitboard) {
        return bitboard & (bitboard - 1);
    }

    /**
     * @brief Print a bitboard for visualization purposes.
     *
     * @param bitboard
     */
    void print_bitboard(Bitboard bitboard);
} // namespace Brainiac