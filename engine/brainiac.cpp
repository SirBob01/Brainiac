#include "brainiac.h"

namespace chess {
    Brainiac::Brainiac() {
        _max_depth = 4;

        hits = 0;
        total = 0;
        visited = 0;
    }

    int Brainiac::search(Board &board, MinimaxNode node, Color player) {
        total++;
        if (_transpositions.contains(board)) {
            // Use transposition table
            hits++;
            return _transpositions.get(board);
        } else if (node.depth == 0) {
            // Use estimate
            return evaluate(board, player);
        } else if (board.is_checkmate()) {
            // Found a winner
            int true_score = board.get_turn() != player ? INT32_MAX : INT32_MIN;
            _transpositions.set(board, node.depth, true_score);
            return true_score;
        }
        visited++;

        // Get next turn and current alpha/beta value
        Color next_turn = static_cast<Color>(!node.turn);
        int value = node.turn == player ? INT32_MIN : INT32_MAX;

        // Set minimax values for successor nodes
        MinimaxNode new_node;
        new_node.depth = node.depth - 1;
        new_node.alpha = node.alpha;
        new_node.beta = node.beta;
        new_node.turn = next_turn;

        // Alpha beta pruning
        const std::vector<Move> &moves = board.get_moves();
        for (int i = 0; i < moves.size(); i++) {
            const Move &move = moves[i];
            new_node.move = move;

            if (node.turn == player) {
                board.execute_move(move);
                value = std::max(value, search(board, new_node, player));
                board.undo_move();

                // From the opponent's perspective, this move will lead
                // to a state that is better for the player than current
                // find
                if (value >= new_node.beta)
                    break;
                new_node.alpha = std::max(new_node.alpha, value);
            } else {
                board.execute_move(move);
                value = std::min(value, search(board, new_node, player));
                board.undo_move();

                // From the player's perspective, this move will lead
                // to a state that is worse than the current find
                if (value <= new_node.alpha)
                    break;
                new_node.beta = std::min(new_node.beta, value);
            }
        }

        // Update the transposition table
        if (node.depth == _max_depth) {
            _transpositions.set(board, node.depth, value);
        }
        return value;
    }

    int Brainiac::evaluate(Board &board, Color maximizing_player) {
        int material = board.calculate_material();
        if (maximizing_player == Color::Black) {
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

        for (auto &move : moves) {
            // Create initial minimax node
            MinimaxNode node;
            node.depth = _max_depth;
            node.alpha = INT32_MIN;
            node.beta = INT32_MAX;
            node.move = move;
            node.turn = player;

            // Get the value of the resulting board state
            board.execute_move(move);
            int value = search(board, node, player);
            board.undo_move();

            // Select best move
            if (value > best_value) {
                best_value = value;
                best_move = move;
            }
        }

        // DEBUGGING - Calculate search time
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        std::cout << "Done searching."
                  << " (" << duration.count() / 1000000.0 << " s)\n";
        std::cout << hits << " hits, " << visited << " searched, " << total
                  << " total.\n";
        return best_move;
    }
} // namespace chess