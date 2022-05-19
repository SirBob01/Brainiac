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

#define MAX_SCORE            100000.0f
#define SECONDS_TO_NANO      1000000000.0f
#define MAX_DEPTH            128
#define MAX_QUIESCENCE_DEPTH 32
#define FUTILITY_MARGIN      100

namespace brainiac {
    using Time = std::chrono::time_point<std::chrono::steady_clock>;

    const uint64_t LMR_MOVE_FILTER =
        MoveFlag::Capture | MoveFlag::BishopPromo | MoveFlag::KnightPromo |
        MoveFlag::QueenPromo | MoveFlag::RookPromo | MoveFlag::EnPassant |
        MoveFlag::Castling;

    /**
     * Initialize engine
     */
    inline void init() {
        init_rook_tables();
        init_bishop_tables();
        init_king_tables();
        init_knight_tables();
    }

    /**
     * Core algorithm for strategically finding the best
     * next move
     */
    class Search {
        double _iterative_timeout_ns;

        // Index by [piece type][to]
        int _history_heuristic[64][64] = {0};

        // Index by depth
        Move _killer_moves[MAX_DEPTH + 1];

        Transpositions _transpositions;
        Time _start_time;

        // Statistic for measuring search pruning performance
        int _visited;

        // Pairing of move with its heuristic score for ordering
        struct MoveScore {
            Move move;
            float score;

            MoveScore() : move({}), score(-INFINITY){};

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
        float ordering_heuristic(Board &board, const Move &move, int depth);

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

        /**
         * Get the current principal variation
         */
        std::vector<Move> get_principal_variation(Board &board);
    };
} // namespace brainiac

#endif