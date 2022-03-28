#ifndef CHESS_BRAINIAC_H_
#define CHESS_BRAINIAC_H_

#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "board.h"
#include "transpositions.h"

namespace chess {
    struct MinimaxNode {
        int depth;
        int alpha;
        int beta;
        Move move;
        Color turn;
    };

    class Brainiac {
        int _max_depth;
        int _max_quiescence_depth;
        Transpositions _transpositions;

        int hits, total, visited;

        /**
         * Convert the board into a 66x1 row-vector
         * as input for the neural network.
         */
        std::vector<double> vectorize(Board &board);

        /**
         * Alpha-beta pruning algorithm with quiescence search
         */
        int alphabeta(Board &board, MinimaxNode node, Color player,
                      bool quiescence = false);

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