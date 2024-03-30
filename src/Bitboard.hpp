#pragma once

#include <cstdint>

namespace Brainiac {
    /**
     * @brief Represent the 8x8 board occupancy as a 64 bit integer.
     *
     */
    using Bitboard = uint64_t;

    /**
     * @brief Count the number of set bits.
     *
     * @param bitboard
     * @return unsigned
     */
    unsigned count_set_bitboard(Bitboard bitboard);

    /**
     * @brief Pop the least significant bit.
     *
     * @param bitboard
     * @return Bitboard
     */
    Bitboard pop_lsb_bitboard(Bitboard bitboard);
} // namespace Brainiac