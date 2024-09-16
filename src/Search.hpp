#pragma once

#include <atomic>
#include <chrono>
#include <functional>

#include "Evaluation.hpp"
#include "History.hpp"
#include "Numeric.hpp"
#include "Position.hpp"
#include "Transpositions.hpp"
#include "Utils.hpp"

namespace Brainiac {
    /**
     * @brief Maximum depth of quiescence search.
     *
     */
    constexpr Depth MAX_QSEARCH_DEPTH = 6;

    /**
     * @brief Search information.
     *
     */
    struct SearchInfo {
        /**
         * @brief Best move found.
         *
         */
        Move move;

        /**
         * @brief Move number.
         *
         */
        unsigned move_number;

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

        /**
         * @brief Search depth.
         *
         */
        Depth depth;

        /**
         * @brief Current valuation in centipawns.
         *
         */
        Value value;
    };

    /**
     * @brief Search limit parameters.
     *
     */
    struct SearchLimits {
        Seconds wtime = Seconds(0);
        Seconds btime = Seconds(0);

        Seconds winc = Seconds(0);
        Seconds binc = Seconds(0);

        Seconds movetime = Seconds(15);

        Depth depth = MAX_DEPTH;

        unsigned nodes = 0;
        unsigned perft = 0;
        unsigned movestogo = 0;
    };

    /**
     * @brief Best move found callback.
     *
     */
    using BestMoveCallback = std::function<void(Move &)>;

    /**
     * @brief Traversal callback.
     *
     */
    using TraverseCallback = std::function<void(SearchInfo &)>;

    /**
     * @brief Search engine.
     *
     */
    class Search {
        Transpositions _tptable;
        History _htable;

        bool _timeout;
        Seconds _start_time;
        Seconds _limit_time;

        unsigned _negamax_visited;
        unsigned _qsearch_visited;

        BestMoveCallback _on_bestmove = [](Move) {};
        TraverseCallback _on_traverse = [](SearchInfo) {};

        std::atomic_bool _running = false;

        /**
         * @brief Static exchange evaluation on a target square.
         *
         * @param position
         * @param target
         * @param pos
         * @return Value
         */
        Value see_target(Position &position, Square target);

        /**
         * @brief Evaluate a capture move.
         *
         * @param move
         * @param pos
         * @return MoveValue
         */
        MoveValue evaluate_capture(Position &position, Move move);

        /**
         * @brief Compute the move value for ordering
         *
         * @param move
         * @param node
         * @return MoveValue
         */
        MoveValue evaluate_move(Position &position, Move move, Node node);

        /**
         * @brief Check if a move can be reduced.
         *
         * @param move
         * @param value
         * @return true
         * @return false
         */
        bool can_reduce_move(Move move, MoveValue value);

        /**
         * @brief Recursive negamax algorithm.
         *
         * @param position
         * @param prev
         * @param depth
         * @param alpha
         * @param beta
         * @param qsearch
         * @return Value
         */
        Value negamax(Position &position,
                      Move prev,
                      Depth depth,
                      Value alpha = MIN_VALUE,
                      Value beta = MAX_VALUE,
                      bool qsearch = false);

      public:
        /**
         * @brief Reset the search state.
         *
         */
        void reset();

        /**
         * @brief Set the bestmove callback.
         *
         * @param callback
         */
        void set_bestmove_callback(BestMoveCallback callback);

        /**
         * @brief Set the traverse callback.
         *
         * @param callback
         */
        void set_traverse_callback(TraverseCallback callback);

        /**
         * @brief Calculate the next viable move for the current turn.
         *
         * Consecutive calls will be ignored unless stop() is invoked to
         * terminate the existing search.
         *
         * @param position
         * @param limits
         * @return Move
         */
        void go(Position &position, SearchLimits limits);

        /**
         * @brief Stop the current search.
         *
         */
        void stop();
    };
} // namespace Brainiac