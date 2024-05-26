#pragma once

#include <chrono>

#include "Evaluation.hpp"
#include "History.hpp"
#include "Numeric.hpp"
#include "Position.hpp"
#include "Transpositions.hpp"
#include "Utils.hpp"

namespace Brainiac {
    /**
     * @brief Search result.
     *
     */
    struct Result {
        /**
         * @brief Best move found.
         *
         */
        Move move;

        /**
         * @brief Total search time.
         *
         */
        Seconds time;

        /**
         * @brief Number of nodes visited.
         *
         */
        unsigned visited;
    };

    /**
     * @brief Search engine.
     *
     */
    class Search {
        Transpositions _tptable;
        History _htable;
        Result _result;

        /**
         * @brief Compute the move score for ordering
         *
         * @param move
         * @param node
         * @return int
         */
        int score_move(Move move, Node node);

        /**
         * @brief Recursive negamax algorithm.
         *
         * @param pos
         * @param depth
         * @param alpha
         * @param beta
         * @return Value
         */
        Value negamax(Position &pos,
                      Depth depth,
                      Value alpha = MIN_VALUE,
                      Value beta = MAX_VALUE);

      public:
        /**
         * @brief Calculate the next viable move for the current turn.
         *
         * @param pos
         * @return Result
         */
        Result search(Position &pos);
    };
} // namespace Brainiac