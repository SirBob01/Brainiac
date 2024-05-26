#pragma once

#include <array>

#include "Move.hpp"
#include "Numeric.hpp"

namespace Brainiac {
    /**
     * @brief History heuristic table for moves ordering.
     *
     */
    class History {
        std::array<int, 64 * 64> _table = {0};

        /**
         * @brief Compute the table index of a move.
         *
         * @param move
         * @return unsigned
         */
        unsigned index(Move move) const;

      public:
        /**
         * @brief Get the score of a move.
         *
         * @param move
         * @return int
         */
        int get(Move move) const;

        /**
         * @brief Update the score of a move.
         *
         * @param move
         * @param depth
         */
        void set(Move move, Depth depth);
    };
} // namespace Brainiac