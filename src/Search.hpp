#pragma once

#include <chrono>

#include "Evaluation.hpp"
#include "History.hpp"
#include "Position.hpp"
#include "Transpositions.hpp"
#include "Utils.hpp"

namespace Brainiac {
    /**
     * @brief Maximum possible depth of the search.
     *
     */
    constexpr unsigned MAX_DEPTH = 7;

    /**
     * @brief Search engine.
     *
     */
    class Search {
        Transpositions _tptable;
        History _htable;

        /**
         * @brief Compute the move score for ordering
         *
         * @param move
         * @param node
         * @return int
         */
        int score_move(Move move, TableEntry node);

        /**
         * @brief Recursive negamax algorithm.
         *
         * @param pos
         * @param depth
         * @param alpha
         * @param beta
         * @return short
         */
        int negamax(Position &pos,
                    unsigned depth,
                    int alpha = LOWER_BOUND,
                    int beta = UPPER_BOUND);

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