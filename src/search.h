#ifndef BRAINIAC_SEARCH_H_
#define BRAINIAC_SEARCH_H_

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

#include "board.h"
#include "heuristic.h"
#include "transpositions.h"

#define MAX_SCORE       100000.0f
#define SECONDS_TO_NANO 1000000000.0f

namespace brainiac {
    using Time = std::chrono::time_point<std::chrono::steady_clock>;

    const uint64_t LMS_MOVE_FILTER =
        MoveFlag::Capture | MoveFlag::BishopPromo | MoveFlag::KnightPromo |
        MoveFlag::QueenPromo | MoveFlag::RookPromo | MoveFlag::EnPassant |
        MoveFlag::Castling;

    /**
     * Initialize engine
     */
    inline void init() {
        init_rook_tables();
        init_bishop_tables();
    }

    /**
     * Core algorithm for strategically finding the best
     * next move
     */
    class Search {
        int _max_depth;
        int _max_quiescence_depth;
        double _iterative_timeout_ns;
        Transpositions _transpositions;
        Time _start_time;

        // Statistic for measuring search pruning performance
        int _visited;

        // Pairing of move with its heuristic score for ordering
        struct MoveScore {
            Move move;
            float score;

            MoveScore(const Move &move, float score) :
                move(move), score(score){};
        };

        /**
         * Alpha-beta pruning algorithm with quiescence search
         */
        float negamax(Board &board,
                      float alpha,
                      float beta,
                      int depth,
                      Color turn,
                      Move &move);

        /**
         * Root of the negamax routine with iterative deepening
         */
        float negamax_root(Board &board, Move &move);

        /**
         * Estimate the value of a move to optimize search in
         * alpha-beta pruning
         */
        float ordering_heuristic(Board &board, const Move &move);

      public:
        Search();

        /**
         * Evaluate the board position relative to White
         */
        float evaluate(Board &board);

        /**
         * Selects the best possible move for the current position
         */
        Move move(Board &board);
    };
} // namespace brainiac

#endif