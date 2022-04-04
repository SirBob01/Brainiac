#include "brainiac.h"

namespace chess {
    Brainiac::Brainiac() {
        _max_depth = 4;
        _max_quiescence_depth = 4;
        _visited = 0;
    }

    float Brainiac::search(Board &board, SearchNode &node) {
        _visited++;
        // TODO: Integrate transposition tables and iterative deepening
        // Terminal conditions
        if (board.is_checkmate()) {
            return board.get_turn() != node.turn ? MAX_SCORE : -MAX_SCORE;
        } else if (board.is_draw()) {
            return 0;
        } else if ((node.depth == -_max_quiescence_depth) ||
                   (node.depth <= 0 && (node.move.flags & MoveFlag::Quiet))) {
            return evaluate(board, node.turn);
        }

        // Set parameters for successor nodes
        SearchNode new_node;
        new_node.depth = node.depth - 1;
        new_node.alpha = -node.beta;
        new_node.beta = -node.alpha;
        new_node.turn = static_cast<Color>(!node.turn);

        // Get move list
        const std::vector<Move> &moves = board.get_moves();
        int n = moves.size();

        // Map moves to heuristic scores for move ordering
        std::vector<MoveScore> move_scores;
        for (const Move &move : moves) {
            float score = ordering_heuristic(board, move);
            move_scores.emplace_back(move, score);
        }

        // Prioritize better moves to optimize pruning with selection sort
        float value = -MAX_SCORE;
        float alpha = node.alpha;
        for (int i = 0; i < n - 1; i++) {
            int best_index = i;
            for (int j = i + 1; j < n; j++) {
                if (move_scores[j].score > move_scores[best_index].score) {
                    best_index = j;
                }
            }
            if (i != best_index) {
                std::swap(move_scores[i], move_scores[best_index]);
            }
            Move &move = move_scores[i].move;
            new_node.move = move;

            // Negamax
            board.execute_move(move);
            value = std::max(value, -search(board, new_node));
            alpha = std::max(value, alpha);
            board.undo_move();
            if (alpha >= node.beta) {
                break;
            }
        }
        return value;
    }

    float Brainiac::ordering_heuristic(Board &board, const Move &move) {
        return mmv_lva_heuristic(board, move);
    }

    float Brainiac::evaluate(Board &board, Color maximizer) {
        float material = material_score(board, maximizer);
        float placement = placement_score(board, maximizer);
        return 0.3 * material + 0.7 * placement;
    }

    Move Brainiac::move(Board &board) {
        Color player = board.get_turn();
        std::vector<Move> moves = board.get_moves();
        Move best_move = moves[0];
        float best_value = -MAX_SCORE;

        auto start = std::chrono::high_resolution_clock::now();

        for (auto &move : moves) {
            // Create initial search node
            SearchNode node;
            node.depth = _max_depth;
            node.alpha = -MAX_SCORE;
            node.beta = MAX_SCORE;
            node.move = move;
            node.turn = player;

            // Get the value of the resulting board state
            board.execute_move(move);
            float value = search(board, node);
            board.undo_move();

            // Select best move
            if (value > best_value) {
                best_value = value;
                best_move = move;
            }
        }

        // DEBUGGING - Calculate search time and nodes visited
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        std::cout << _visited << " nodes searched"
                  << " (" << duration.count() / 1000000.0 << " s)\n";
        return best_move;
    }
} // namespace chess