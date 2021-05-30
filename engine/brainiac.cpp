#include "brainiac.h"

namespace chess {
    Brainiac::Brainiac(Color color) {
        neural::NetworkParameters params;
        params.random_range = 1.0;
        params.learning_rate = 0.1;
        params.cost_function = neural::quadratic_cost;
        params.layers = {
            {66, nullptr},
            {128, neural::lrelu},
            {128, neural::lrelu},
            {128, neural::lrelu},
            {1, neural::tanh},
        };
        _network = std::make_unique<neural::Network>(params);

        _root = new SearchNode();
        _color = color;
    }

    Brainiac::~Brainiac() {
        delete_recur(_root);
    }

    void Brainiac::delete_recur(SearchNode *node) {
        for(auto child : node->children) {
            delete_recur(child.second);
        }
        delete node;
    }

    std::vector<double> Brainiac::vectorize(Board &board) {
        // Stores normalized input vector for neural network
        std::vector<double> vector;

        // All the pieces on the board
        for(int row = 0; row < 8; row++) {
            for(int col = 0; col < 8; col++) {
                Piece piece = board.get_at_coords(row, col);
                if(piece.is_empty()) {
                    vector.push_back(0);
                }
                else {
                    double i = 1+piece.get_piece_index();
                    vector.push_back(i / 12.0);
                }
            }
        }
        
        // Castling rights is (somewhat) independent of the current board states
        vector.push_back(board.get_castling_rights() / 16.0);

        // Enforcing the 50-move rule ensures finite length games
        vector.push_back(board.get_halfmoves() / 100.0);
        
        return vector;
    }

    void Brainiac::train_recur(SearchNode *root, Board &board) {
        // Base case (win/lose/draw)
        if(board.is_draw()) {
            root->score = 0;
            return;
        }
        else if(board.is_checkmate()) {
            if(board.get_turn() != _color) {
                root->score = 1;
            }
            else {
                root->score= -1;
            }
            return;
        }

        // Choose a random move to play at each iteration step
        std::vector<Move> moves = board.get_moves();
        Move move = moves[std::rand()%moves.size()];
        std::string key = move.standard_notation();

        board.execute_move(move);
        if(root->children.count(key)) {
            // Chosen node has been visited
            train_recur(root->children[key], board);
        }
        else {
            // Chosen node is a new move unexplored, so evaluate it
            root->children[key] = new SearchNode();
            train_recur(root->children[key], board);
            
            // Calculate current true score by averaging children's scores
            root->score = 0;
            for(auto &child : root->children) {
                root->score += child.second->score;
            }
            root->score /= root->children.size();

            // Train the network
            std::vector<double> expected = {root->score};
            for(int i = 0; i < 100; i++) {
                _network->fit(vectorize(board), expected);
            }
        }
        board.undo_move();
    }

    void Brainiac::train() {
        /**
         * Algorithm 1. Single game train:
         * 1. Loop until reaching terminal node (win/lose/draw)
         *      a. Generate move space for current board state
         *      b. Select random move from move space
         *      c. Execute move
         * 2. Loop until reaching initial root node
         *      a. Train the network (expected = calculated score of board = average scores of children)
         *      b. Undo move
         */
        Board board;
        train_recur(_root, board);
    }

    Move Brainiac::evaluate(Board &board) {
        std::vector<Move> moves = board.get_moves();
        // Lowest possible score is when you lose
        double best_score = -1.0;
        Move best_move;

        for(auto &move : moves) {
            board.execute_move(move);
            neural::Matrix output = _network->evaluate(vectorize(board));
            double score = output.get_at(0, 0);
            if(score >= best_score) {
                best_score = score;
                best_move = move;
            }
            board.undo_move();
        }
        return best_move;
    }
}