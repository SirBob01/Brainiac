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
     * 12 piece types (6 per color) and en-passant target for the 64 squares.
     * Current turn state.
     * 4 castling flags.
     *
     */
    const unsigned BITSTRING_COUNT = (64 * 13) + 1 + 4;

    /**
     * @brief Zobrist hasher functor.
     *
     */
    class Hasher {
        std::array<Hash, BITSTRING_COUNT> _bitstrings;

      public:
        /**
         * @brief Initialize the hasher with a random seed.
         *
         * @param seed
         */
        Hasher(unsigned seed = time(0));

        /**
         * @brief Get the bitstring for a square / piece combination.
         *
         * @param square
         * @param piece
         * @return Hash
         */
        Hash bitstring(Square square, Piece piece) const;

        /**
         * @brief Get the bitstring for an en passant target square.
         *
         * @param ep_dst
         * @return Hash
         */
        Hash bitstring(Square ep_dst) const;

        /**
         * @brief Get the bitstring for the current color turn.
         *
         * @param turn
         * @return Hash
         */
        Hash bitstring(Color turn) const;

        /**
         * @brief Get the bitstring for a castling right.
         *
         * @param right
         * @return Hash
         */
        Hash bitstring(CastlingRight right) const;

        /**
         * @brief State hashing function.
         *
         * @param board
         * @param castling
         * @param turn
         * @param ep_dst
         * @return Hash
         */
        Hash operator()(Board &board,
                        CastlingFlagSet castling,
                        Color turn,
                        Square ep_dst);
    };
} // namespace Brainiac