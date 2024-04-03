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
    constexpr const std::array<Bitboard, 8> RANKS = {
        0x00000000000000FF,
        0x000000000000FF00,
        0x0000000000FF0000,
        0x00000000FF000000,
        0x000000FF00000000,
        0x0000FF0000000000,
        0x00FF000000000000,
        0xFF00000000000000,
    };

    /**
     * @brief File masks.
     *
     */
    constexpr const std::array<Bitboard, 8> FILES = {
        0x0101010101010101,
        0x0202020202020202,
        0x0404040404040404,
        0x0808080808080808,
        0x1010101010101010,
        0x2020202020202020,
        0x4040404040404040,
        0x8080808080808080,
    };

    /**
     * @brief Diagonal mask.
     *
     */
    constexpr Bitboard MAIN_DIAGONAL = 0x8040201008040201;

    /**
     * @brief Anti-diagonal mask.
     *
     */
    constexpr Bitboard ANTI_DIAGONAL = 0x0102040810204080;

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