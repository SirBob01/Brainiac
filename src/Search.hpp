#pragma once

#include <chrono>

#include "Evaluation.hpp"
#include "Position.hpp"
#include "Transpositions.hpp"
#include "Utils.hpp"

namespace Brainiac {
    /**
     * @brief Maximum possible depth of the search.
     *
     */
    constexpr short MAX_DEPTH = 7;

    /**
     * @brief Search engine.
     *
     */
    class Search {
        Transpositions _tptable;

        /**
         * @brief Recursive negamax algorithm.
         *
         * @param pos
         * @param depth
         * @param alpha
         * @param beta
         * @return short
         */
        short negamax(Position &pos,
                      short depth,
                      short alpha = LOWER_BOUND,
                      short beta = UPPER_BOUND);

      public:
        /**
         * @brief Calculate the next viable move for the current turn.
         *
         * @param pos
         * @return Move
         */
        Move move(Position &pos);
    };
} // namespace Brainiac