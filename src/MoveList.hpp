#pragma once

#include <array>

#include "Move.hpp"

namespace Brainiac {
    /**
     * @brief Theoretical maximum number of moves playable in a chess game.
     *
     */
    constexpr unsigned MAX_MOVES_PER_TURN = 500ULL;

    /**
     * @brief A stack-allocated list of moves
     *
     */
    class MoveList {
        std::array<Move, MAX_MOVES_PER_TURN> _list;
        unsigned _count = 0;

      public:
        /**
         * @brief Access a move in the list
         *
         * @param index
         * @return Move
         */
        Move operator[](unsigned index) const;

        /**
         * @brief Get the size of the list
         *
         * @return unsigned
         */
        unsigned size() const;

        /**
         * @brief Clear the list
         *
         */
        void clear();

        /**
         * @brief Add a new move to the list
         *
         * @param src
         * @param dst
         * @param type
         */
        void add(Square src, Square dst, MoveType type);

        /**
         * @brief Get the pointer to the first move in the list
         *
         * @return const Move*
         */
        const Move *begin() const;

        /**
         * @brief Get the pointer to after the end of the list
         *
         * @return const Move*
         */
        const Move *end() const;
    };
} // namespace Brainiac