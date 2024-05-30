#pragma once

#include <array>

#include "Move.hpp"
#include "Numeric.hpp"
#include "Piece.hpp"
#include "Position.hpp"

namespace Brainiac {
    /**
     * @brief History heuristic table for moves ordering.
     *
     */
    class History {
        std::array<MoveValue, 12 * 64> _table;

        /**
         * @brief Compute the table index of a move.
         *
         * @param position
         * @param move
         * @return unsigned
         */
        unsigned index(const Position &position, Move move) const;

      public:
        History();

        /**
         * @brief Get the score of a move.
         *
         * @param position
         * @param move
         * @return MoveValue
         */
        MoveValue get(const Position &position, Move move) const;

        /**
         * @brief Update the score of a move.
         *
         * @param position
         * @param move
         * @param depth
         */
        void set(const Position &position, Move move, Depth depth);

        /**
         * @brief Clear the table.
         *
         */
        void clear();
    };
} // namespace Brainiac