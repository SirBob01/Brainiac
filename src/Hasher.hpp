#pragma once

#include <cstdint>
#include <random>
#include <vector>

#include "Board.hpp"
#include "Move.hpp"

namespace Brainiac {
    /**
     * @brief State hash bitfield.
     *
     */
    using Hash = uint64_t;

    /**
     * @brief Number of bitstrings.
     *
     */
    const unsigned BITSTRING_COUNT = (64 * 12) + 1 + 4 + 1;

    /**
     * @brief Zobrist hasher function.
     *
     */
    struct Hasher {
        std::vector<Hash> bitstrings;

        /**
         * @brief Initialize the hasher with a random seed.
         *
         * @param seed
         */
        Hasher(unsigned seed = time(0));

        /**
         * @brief Hash some game state.
         *
         * @param board
         * @param castling
         * @param turn
         * @param ep_target
         * @return Hash
         */
        Hash hash(Board &board,
                  CastlingFlagSet castling,
                  Color turn,
                  Square ep_target);
    };
} // namespace Brainiac