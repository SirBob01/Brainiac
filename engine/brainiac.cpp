#include "brainiac.h"

namespace chess {
    Brainiac::Brainiac() {
        _max_depth = 5;
        _max_quiescence_depth = 1;
        hits = 0;
        total = 0;
        visited = 0;
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
        // Read from the transposition table
        total++;
        if(_transpositions.contains(board)) {
            hits++;
            return _transpositions.get(board);
        }
        visited++;

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
            Piece mvv_piece = board.get_at(move.to);
            Piece lva_piece = board.get_at(move.from);
            board.execute_move(move);
            
            int mvv = 0;
            int lva = 0;
            if(!mvv_piece.is_empty()) {
                mvv = piece_weights[mvv_piece.get_piece_index()];
            }
            if(!lva_piece.is_empty()) {
                lva = piece_weights[lva_piece.get_piece_index()];
            }
            if(mvv_piece.color == Color::Black) {
                mvv *= -1;
            }
            move_scores.push_back(std::make_pair(move, mvv - lva));
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
            auto &move = move_scores[i].first;

            board.execute_move(move);
            MinimaxNode new_node = {node.depth-1, node.alpha, node.beta, move, next_turn};
            int child_value = alphabeta(board, new_node, player, quiescence);
            board.undo_move();

            if(node.turn == player) {
                value = std::max(value, child_value);
                node.alpha = std::max(node.alpha, value);
                if(value >= node.beta) {
                    break;
                }
            }
            else {
                value = std::min(value, child_value);
                node.beta = std::min(node.beta, value);
                if(value <= node.alpha) {
                    break;
                }
            }
        }

        // Update the transposition table
        if(node.depth == _max_depth) {
            _transpositions.set(board, node.depth, value);
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

        auto start = std::chrono::high_resolution_clock::now();
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
        std::cout << "Done searching." << " (";
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        std::cout << duration.count()/1000000.0 << " s)\n";
        std::cout << hits << " hits, " << visited << " searched, " << total << " total.\n";
        _transpositions.print();
        return best_move;
    }
}