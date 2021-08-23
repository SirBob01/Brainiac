#ifndef CHESS_BRAINIAC_H_
#define CHESS_BRAINIAC_H_

#include <vector>
#include <unordered_map>
#include <memory>
#include <thread>
#include <mutex>

#include "board.h"

namespace chess {
    struct MinimaxNode {
        int depth;
        int alpha;
        int beta;
        Move move;
        Color turn;
    };

    class Brainiac {
        int max_depth;
        int max_quiescence_depth;

        /**
         * Convert the board into a 66x1 row-vector
         * as input for the neural network.
         */
        std::vector<double> vectorize(Board &board);

        /**
         * Alpha-beta pruning algorithm with quiescence search
         */
        int alphabeta(Board &board, MinimaxNode node, Color player, bool quiescence=false);

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
}

#endif