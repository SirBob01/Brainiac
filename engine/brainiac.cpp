#include "brainiac.h"

namespace chess {
    Brainiac::Brainiac() {
        max_depth = 5;
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

    int Brainiac::alphabeta(Board &board, MinimaxNode node, Color player) {
        // TODO: Sort moves and perform quiescence search
        std::vector<Move> moves = board.get_moves();
        if(node.depth == 0 || moves.size() == 0) {
            int heuristic = evaluate(board, player);
            return heuristic;
        }

        Color next_turn = static_cast<Color>(!node.turn);
        int value;
        if(node.turn == player) {
            value = INT32_MIN;
        }
        else {
            value = INT32_MAX;
        }
        for(auto &move : moves) {
            board.execute_move(move);
            MinimaxNode new_node = {node.depth-1, node.alpha, node.beta, next_turn};
            int recur_value = alphabeta(board, new_node, player);
            board.undo_move();

            if(node.turn == player) {
                value = std::max(value, recur_value);
                if(value >= node.beta) {
                    break;
                }
                node.alpha = std::max(node.alpha, value);
            }
            else {
                value = std::min(value, recur_value);
                if(value <= node.alpha) {
                    break;
                }
                node.beta = std::min(node.beta, value);
            }
        }
        return value;
    }

    int Brainiac::evaluate(Board &board, Color maximizing_player) {
        // TODO: Calculate using neural network
        int material = board.calculate_material();
        if(maximizing_player == Color::Black) {
            return -material;
        }
        return material;
    }

    Move Brainiac::move(Board &board) {
        Color player = board.get_turn();
        std::vector<Move> moves = board.get_moves();
        int best_value = INT32_MIN;
        Move best_move = moves[0];

        for(auto &move : moves) {
            board.execute_move(move);
            int value = alphabeta(board, {max_depth, INT32_MIN, INT32_MAX, player}, player);
            board.undo_move();
            if(value > best_value) {
                best_value = value;
                best_move = move;
            }
        }
        return best_move;
    }
}