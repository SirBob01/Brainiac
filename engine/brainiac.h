#ifndef CHESS_BRAINIAC_H_
#define CHESS_BRAINIAC_H_

#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "board.h"
#include "heuristic.h"
#include "transpositions.h"

#define MAX_SCORE 1000.0f

namespace chess {
    /**
     * Initialize engine
     */
    inline void init() {
        init_rook_tables();
        init_bishop_tables();
    }

    /**
     * Contains per-node state data required during alpha-beta minimax
     */
    struct SearchNode {
        int depth;
        float alpha;
        float beta;
        Move move;
        Color turn;
    };

    /**
     * Core algorithm for strategically finding the best
     * next move
     */
    class Brainiac {
        int _max_depth;
        int _max_quiescence_depth;
        Transpositions _transpositions;

        // Statistic for measuring search pruning performance
        int _visited;

        // Pairing of move with its heuristic score for ordering
        struct MoveScore {
            Move move;
            float score;

            MoveScore(const Move &move, float score)
                : move(move), score(score){};
        };

        /**
         * Alpha-beta pruning algorithm with quiescence search
         */
        float search(Board &board, SearchNode node);

        /**
         * Estimate the value of a move to optimize search in
         * alpha-beta pruning
         */
        float ordering_heuristic(Board &board, const Move &move);

      public:
        Brainiac();

        /**
         * Calculates the score of the maximizing player based
         * on the board state
         */
        float evaluate(Board &board, Color maximizing_player);

        /**
         * Selects the best possible move for the current position
         */
        Move move(Board &board);
    };
} // namespace chess

#endif