#ifndef CHESS_BRAINIAC_H_
#define CHESS_BRAINIAC_H_

#include <vector>
#include <unordered_map>

#include "neural/network.h"
#include "neural/activations.h"
#include  "neural/costs.h"
#include "board.h"

namespace chess {
    struct SearchNode {
        double score = 0.0;

        // Moves -> Children
        std::unordered_map<std::string, SearchNode*> children;
    };

    class Brainiac {
        std::unique_ptr<neural::Network> _network;
        Color _color;

        SearchNode *_root;

        /**
         * Convert the board into a 66x1 row-vector
         * as input for the neural network.
         */
        std::vector<double> vectorize(Board &board);

        /**
         * Perform one episode of the Monte-Carlo tree search 
         * algorithm and train the neural network at each 
         * backtracking step
         */
        void train_recur(SearchNode *root, Board &board);

        /**
         * Recursively delete the search tree
         */
        void delete_recur(SearchNode *node);

    public:
        Brainiac(Color color);
        ~Brainiac();

        /**
         * Trains by playing games against itself and updating its neural network.
         * This network calculates the probability of winning at a particular board state.
         */
        void train();

        /**
         * Evaluates the board state and selects the best
         * possible move
         */
        Move evaluate(Board &board);
    };
}

#endif