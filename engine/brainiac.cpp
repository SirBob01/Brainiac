#include "brainiac.h"

namespace chess {
    Brainiac::Brainiac() {
        _max_depth = 4;
        _max_quiescence_depth = 2;
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

    int Brainiac::alphabeta(Board &board, MinimaxNode node, Color player, bool quiescence) {
        std::vector<Move> moves = board.get_moves();
        int n = moves.size();
        if(!quiescence) {
            if(n == 0) {
                // Return a static evaluation of the board
                return evaluate(board, player);
            }
            if(node.depth == 0) {
                // Search further until a quiet move is found
                node.depth = _max_quiescence_depth;
                return alphabeta(board, node, player, true);
            }
        }
        else {
            bool quiet = (node.move.flags & MoveFlag::Quiet);
            if(quiet || n == 0 || node.depth == 0) {
                return evaluate(board, player);
            }
        }
        
        // Read from the transposition table
        if(_transpositions.contains(board, node.depth)) {
            return _transpositions.get(board, node.depth);
        }

        Color next_turn = static_cast<Color>(!node.turn);
        int value;
        if(node.turn == player) {
            value = INT32_MIN;
        }
        else {
            value = INT32_MAX;
        }

        std::vector<std::pair<Move, int>> move_scores;
        for(auto &move : moves) {
            board.execute_move(move);
            int score = board.calculate_material();
            if(player == Color::Black) {
                score *= -1;
            }
            move_scores.push_back(std::make_pair(move, score));
            board.undo_move();
        }

        // Move ordering allows early termination search
        for(int i = 0; i < n - 1; i++) {
            int max_score = move_scores[i].second;
            int max_index = i;
            for(int j = i + 1; j < n; j++) {
                if(move_scores[j].second > max_score) {
                    max_score = move_scores[j].second;
                    max_index = j;
                }
            }
            std::swap(move_scores[i], move_scores[max_index]);
            auto move = move_scores[i].first;

            board.execute_move(move);
            MinimaxNode new_node = {node.depth-1, node.alpha, node.beta, move, next_turn};
            int child_value = alphabeta(board, new_node, player, quiescence);
            board.undo_move();

            if(node.turn == player) {
                value = std::max(value, child_value);
                if(value >= node.beta) {
                    break;
                }
                node.alpha = std::max(node.alpha, value);
            }
            else {
                value = std::min(value, child_value);
                if(value <= node.alpha) {
                    break;
                }
                node.beta = std::min(node.beta, value);
            }
        }

        // Update the transposition table
        _transpositions.set(board, node.depth, value);
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
            MinimaxNode node = {_max_depth, INT32_MIN, INT32_MAX, move, player};
            int value = alphabeta(board, node, player);
            board.undo_move();
            if(value > best_value) {
                best_value = value;
                best_move = move;
            }
        }
        return best_move;
    }
}