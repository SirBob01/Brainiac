#include "brainiac.h"

namespace chess {
    Brainiac::Brainiac() {
        _max_depth = 128;
        _max_quiescence_depth = 16;
        _iterative_timeout_ns = 0.75 * SECONDS_TO_NANO;
        _visited = 0;
    }

    float Brainiac::negamax(Board &board, SearchNode node) {
        // Read from the transposition table
        float alpha_orig = node.alpha;
        TableNode &entry = _transpositions.get(board);

        // Terminate early and return last known search result
        Time end_time = std::chrono::steady_clock::now();
        if ((end_time - _start_time).count() >= _iterative_timeout_ns) {
            return entry.value;
        }
        if (entry.key == board.get_hash() && entry.depth >= node.depth) {
            switch (entry.type) {
            case NodeType::Exact:
                return entry.value;
                break;
            case NodeType::Lower:
                node.alpha = std::max(node.alpha, entry.value);
                break;
            case NodeType::Upper:
                node.beta = std::min(node.beta, entry.value);
                break;
            default:
                break;
            }
            if (node.alpha >= node.beta) {
                return entry.value;
            }
        }

        // Terminal conditions
        _visited++;
        if (board.is_checkmate()) {
            // Prioritize earlier-found checkmates
            float score = MAX_SCORE + node.depth;
            return board.get_turn() != node.turn ? score : -score;
        } else if (board.is_draw()) {
            return 0;
        } else if ((node.depth == -_max_quiescence_depth) ||
                   (node.depth <= 0 && (node.move.flags & MoveFlag::Quiet))) {
            float score = evaluate(board);
            return node.turn == Color::White ? score : -score;
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
        Move best_move;
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
            value = std::max(value, -negamax(board, new_node));
            node.alpha = std::max(value, node.alpha);
            board.undo_move();
            if (node.alpha >= node.beta) {
                // Hash move to be prioritized in next search iteration
                best_move = move;
                break;
            }
        }

        // Update the transposition table
        TableNode new_entry;
        new_entry.key = board.get_hash();
        new_entry.value = value;
        new_entry.depth = node.depth;
        new_entry.best_move = best_move;
        if (value <= alpha_orig) {
            new_entry.type = NodeType::Upper;
        } else if (value >= node.beta) {
            new_entry.type = NodeType::Lower;
        } else {
            new_entry.type = NodeType::Exact;
        }
        _transpositions.set(board, new_entry);
        return value;
    }

    float Brainiac::search(Board &board, SearchNode &node) {
        _start_time = std::chrono::steady_clock::now();
        float value = -MAX_SCORE;

        // Iterative deepening from an initial depth until it runs out of time
        for (int depth = 1; depth <= _max_depth; depth++) {
            node.depth = depth;
            value = negamax(board, node);

            Time end_time = std::chrono::steady_clock::now();
            if ((end_time - _start_time).count() >= _iterative_timeout_ns) {
                return value;
            }
        }
        return value;
    }

    float Brainiac::ordering_heuristic(Board &board, const Move &move) {
        float score = 0;

        // Hashed move should be evaluated first
        TableNode &node = _transpositions.get(board);
        if (node.best_move == move) {
            score += 1000.0f;
        }

        // Evaluate captures based on SEE heuristic
        if (move.flags & MoveFlag::Capture) {
            score += see_heuristic(board, move);
        }
        return score;
    }

    float Brainiac::evaluate(Board &board) {
        float material = board.get_material();
        float placement = placement_score(board);
        float mobility = mobility_score(board);

        return 1.0 * material + 0.1 * placement + 0.1 * mobility;
    }

    Move Brainiac::move(Board &board) {
        Time start = std::chrono::steady_clock::now();

        Color player = board.get_turn();
        std::vector<Move> moves = board.get_moves();
        Move best_move = moves[0];
        float best_value = -MAX_SCORE;

        _visited = 0;
        for (auto &move : moves) {
            // Create initial search node
            SearchNode node;
            node.alpha = -MAX_SCORE;
            node.beta = MAX_SCORE;
            node.move = move;
            node.turn = static_cast<Color>(!player);

            // Get the value of the resulting board state
            board.execute_move(move);
            float value = -search(board, node);
            board.undo_move();

            // Update best move
            if (value > best_value) {
                best_value = value;
                best_move = move;
            }
        }

        // DEBUGGING - Calculate search time and nodes visited
        Time stop = std::chrono::steady_clock::now();
        auto duration = (stop - start) / SECONDS_TO_NANO;
        std::cout << _visited << " nodes searched"
                  << " (" << duration.count() << " s)\n";
        return best_move;
    }
} // namespace chess