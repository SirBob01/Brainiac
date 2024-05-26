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

        Seconds _start_time;
        unsigned _visited;

        /**
         * @brief Static exchange evaluation on a target square.
         *
         * @param target
         * @param pos
         * @return Value
         */
        Value see_target(Square target, Position &pos);

        /**
         * @brief Evaluate a capture move.
         *
         * @param move
         * @param pos
         * @return MoveValue
         */
        MoveValue evaluate_capture(Move move, Position &pos);

        /**
         * @brief Compute the move value for ordering
         *
         * @param move
         * @param node
         * @param pos
         * @return MoveValue
         */
        MoveValue evaluate_move(Move move, Node node, Position &pos);

        /**
         * @brief Check if a move can be reduced.
         *
         * @param move
         * @return true
         * @return false
         */
        bool can_reduce_move(Move move, MoveValue value);

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