#pragma once

#include <array>
#include <atomic>
#include <functional>

#include "History.hpp"
#include "Numeric.hpp"
#include "PVTable.hpp"
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
     * @brief Iterative deepening information.
     *
     */
    struct IterativeInfo {
        /**
         * @brief Current move searched.
         *
         */
        Move move;

        /**
         * @brief Move number.
         *
         */
        unsigned move_number;

        /**
         * @brief Search depth.
         *
         */
        Depth depth;
    };

    /**
     * @brief PV information.
     *
     */
    struct PVInfo {
        /**
         * @brief Current depth.
         *
         */
        Depth depth;

        /**
         * @brief Total time spent in search.
         *
         */
        Seconds time;

        /**
         * @brief Total number of nodes traversed.
         *
         */
        unsigned nodes;

        /**
         * @brief Estimated valuation.
         *
         */
        Value value;

        /**
         * @brief PV move list.
         *
         */
        std::array<Move, MAX_DEPTH> pv;

        /**
         * @brief Number of moves in the PV.
         *
         */
        unsigned pv_length;
    };

    /**
     * @brief Search limit parameters.
     *
     */
    struct SearchLimits {
        /**
         * @brief Remaining time for white.
         *
         */
        Seconds white_time = Seconds(0);

        /**
         * @brief Remaining time for black.
         *
         */
        Seconds black_time = Seconds(0);

        /**
         * @brief Time increment per move for white.
         *
         */
        Seconds white_increment = Seconds(0);

        /**
         * @brief Time increment per move for black.
         *
         */
        Seconds black_increment = Seconds(0);

        /**
         * @brief Fixed search time (overrides all other settings).
         *
         */
        Seconds move_time = Seconds(0);

        /**
         * @brief Maximum number of nodes to search.
         *
         */
        unsigned nodes = 0;

        /**
         * @brief Number of moves remaining.
         *
         */
        unsigned moves_to_go = 0;

        /**
         * @brief Maximum depth to search.
         *
         */
        Depth depth = MAX_DEPTH;
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
    using IterativeCallback = std::function<void(IterativeInfo &)>;

    /**
     * @brief PV callback.
     *
     */
    using PVCallback = std::function<void(PVInfo &)>;

    /**
     * @brief Search engine.
     *
     */
    class Search {
        Transpositions _tptable;
        History _htable;
        PVTable _pvtable;

        std::atomic_bool _running;

        bool _timeout;
        Seconds _start_time;
        Seconds _limit_time;

        unsigned _negamax_visited;
        unsigned _qsearch_visited;

        BestMoveCallback _on_bestmove;
        IterativeCallback _on_iterative;
        PVCallback _on_pv;

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
         * @param ply
         * @param alpha
         * @param beta
         * @param qsearch
         * @return Value
         */
        Value negamax(Position &position,
                      Move prev,
                      Depth depth,
                      Depth ply,
                      Value alpha = MIN_VALUE,
                      Value beta = MAX_VALUE,
                      bool qsearch = false);

      public:
        Search();

        /**
         * @brief Reset the search state.
         *
         */
        void reset();

        /**
         * @brief Set the iterative deepening callback.
         *
         * @param callback
         */
        void set_iterative_callback(IterativeCallback callback);

        /**
         * @brief Set the PV callback.
         *
         * @param callback
         */
        void set_pv_callback(PVCallback callback);

        /**
         * @brief Set the bestmove callback.
         *
         * @param callback
         */
        void set_bestmove_callback(BestMoveCallback callback);

        /**
         * @brief Calculate the next viable move for the current turn.
         *
         * Consecutive calls will be ignored unless stop() is invoked to
         * terminate the running search.
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