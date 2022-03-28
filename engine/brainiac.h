#ifndef CHESS_BRAINIAC_H_
#define CHESS_BRAINIAC_H_

#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "board.h"
#include "transpositions.h"

namespace chess {
    /**
     * Contains per-node state data required during alpha-beta minimax
     */
    struct MinimaxNode {
        int depth;
        int alpha;
        int beta;
        Move move;
        Color turn;
    };

    /**
     * Core algorithm for strategically finding the best
     * next move
     */
    class Brainiac {
        int _max_depth;
        Transpositions _transpositions;

        int hits, total, visited;

        /**
         * Alpha-beta pruning algorithm with quiescence search
         */
        int search(Board &board, MinimaxNode node, Color player);

      public:
        Brainiac();

        /**
         * Calculates the score of the maximizing player based
         * on the board state
         */
        int evaluate(Board &board, Color maximizing_player);

        /**
         * Selects the best possible move for the current position
         */
        Move move(Board &board);
    };
} // namespace chess

#endif