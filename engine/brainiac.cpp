#include "brainiac.h"

namespace chess {
    Brainiac::Brainiac() {
        _max_depth = 4;
        _max_quiescence_depth = 4;

        _hits = 0;
        _total = 0;
        _visited = 0;
    }

    int Brainiac::search(Board &board, MinimaxNode node, Color player,
                         bool quiescence) {
        const std::vector<Move> &moves = board.get_moves();
        const size_t n = moves.size();

        // Terminal conditions
        _total++;
        if (_transpositions.contains(board)) {
            // Use transposition table
            _hits++;
            return _transpositions.get(board);
        } else if (node.depth == 0) {
            if (quiescence) {
                // Maximized our quiescence search, return evaluation
                return evaluate(board, player);
            } else {
                // Search further until a quiet move is found
                node.depth = _max_quiescence_depth;
                return search(board, node, player, true);
            }
        } else if (quiescence) {
            // Quiescence search found a quiet move, we can stop early
            bool quiet = (node.move.flags & MoveFlag::Quiet);
            if (quiet || n == 0 || node.depth == 0) {
                return evaluate(board, player);
            }
        } else if (board.is_checkmate()) {
            // Found a winner
            int true_score = board.get_turn() != player ? INT32_MAX : INT32_MIN;
            _transpositions.set(board, node.depth, true_score);
            return true_score;
        }
        _visited++;

        // Get next turn and current alpha/beta value
        Color next_turn = static_cast<Color>(!node.turn);
        int value = node.turn == player ? INT32_MIN : INT32_MAX;

        // Set minimax values for successor nodes
        MinimaxNode new_node;
        new_node.depth = node.depth - 1;
        new_node.alpha = node.alpha;
        new_node.beta = node.beta;
        new_node.turn = next_turn;

        // Move ordering using MVV-LVA heuristic
        std::vector<std::pair<Move, int>> move_scores;
        for (auto &move : moves) {
            Piece mvv_piece = board.get_at(move.to);
            Piece lva_piece = board.get_at(move.from);

            int mvv = 0;
            int lva = 0;
            if (!mvv_piece.is_empty()) {
                mvv = piece_weights[mvv_piece.get_piece_index()];
                if (mvv_piece.color == Color::Black)
                    mvv *= -1;
            }
            if (!lva_piece.is_empty()) {
                lva = piece_weights[lva_piece.get_piece_index()];
                if (lva_piece.color == Color::Black)
                    lva *= -1;
            }
            move_scores.push_back(std::make_pair(move, mvv - lva));
        }

        // Prioritize better moves to optimize pruning
        // Use selection sort
        for (int i = 0; i < n - 1; i++) {
            int max_score = move_scores[i].second;
            int max_index = i;
            for (int j = i + 1; j < n; j++) {
                if (move_scores[j].second > max_score) {
                    max_score = move_scores[j].second;
                    max_index = j;
                }
            }
            std::swap(move_scores[i], move_scores[max_index]);
            Move &move = move_scores[i].first;
            new_node.move = move;

            if (node.turn == player) {
                board.execute_move(move);
                value = std::max(value,
                                 search(board, new_node, player, quiescence));
                board.undo_move();

                // From the opponent's perspective, this move will lead
                // to a state that is better for the player than current
                // find
                if (value >= new_node.beta)
                    break;
                new_node.alpha = std::max(new_node.alpha, value);
            } else {
                board.execute_move(move);
                value = std::min(value,
                                 search(board, new_node, player, quiescence));
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
            int value = search(board, node, player, false);
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
        std::cout << _hits << " hits, " << _visited << " searched, " << _total
                  << " total.\n";
        return best_move;
    }
} // namespace chess