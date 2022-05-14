#include "search.h"

namespace brainiac {
    Search::Search() {
        _max_depth = 50;
        _max_quiescence_depth = 16;
        _iterative_timeout_ns = 0.5 * SECONDS_TO_NANO;
        _visited = 0;
    }

    float Search::negamax(Board &board,
                          float alpha,
                          float beta,
                          int depth,
                          Color turn,
                          Move &move) {
        // Read from the transposition table
        float alpha_orig = alpha;
        TableEntry &entry = _transpositions.get(board);

        // Terminate early and return last known search result
        Time end_time = std::chrono::steady_clock::now();
        if ((end_time - _start_time).count() >= _iterative_timeout_ns) {
            return entry.value;
        }
        if (entry.key == board.get_hash() && entry.depth >= depth) {
            switch (entry.type) {
            case NodeType::Exact:
                return entry.value;
                break;
            case NodeType::Lower:
                alpha = std::max(alpha, entry.value);
                break;
            case NodeType::Upper:
                beta = std::min(beta, entry.value);
                break;
            default:
                break;
            }
            if (alpha >= beta) {
                return entry.value;
            }
        }

        // Terminal conditions
        _visited++;
        if (board.is_checkmate()) {
            // Prioritize earlier-found checkmates
            float score = MAX_SCORE + depth;
            return board.get_turn() != turn ? score : -score;
        } else if (board.is_draw()) {
            return 0;
        } else if ((depth == -_max_quiescence_depth) ||
                   (depth <= 0 && (move.flags & MoveFlag::Quiet))) {
            float score = evaluate(board);
            return turn == Color::White ? score : -score;
        }

        // Get opponent color
        Color opp = static_cast<Color>(!turn);

        // Get move list
        const std::vector<Move> &moves = board.get_moves();
        int n = moves.size();

        // Map moves to heuristic scores for move ordering
        std::vector<MoveScore> move_scores;
        for (const Move &move : moves) {
            float score = ordering_heuristic(board, move);
            move_scores.emplace_back(move, score);
        }

        float value = -INFINITY;
        Move best_move;

        // Prioritize better moves to optimize pruning with selection sort
        for (int i = 0; i < n; i++) {
            // Selection sort assumes the search will cutoff early with a good
            // move ordering heuristic
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

            // Prune bad captures during quiescence search
            if (depth < 0 && move_scores[i].score < 0) {
                continue;
            }

            // PV search with late move reduction
            // Tactical moves should not be reduced
            if (i > 1 && depth >= 3 && !board.is_check()) {
                board.execute_move(move);
                int R = 0;
                if (!(move.flags & LMS_MOVE_FILTER)) {
                    R++;
                    if (i > 6) {
                        R++;
                    }
                }
                float reduction = -negamax(board,
                                           -alpha - 1,
                                           -alpha,
                                           depth - R - 1,
                                           opp,
                                           move);
                board.undo_move();

                // This move is proven to be not good, skip it
                if (reduction < alpha) {
                    continue;
                }
            }

            // Full search negamax
            board.execute_move(move);
            value =
                std::max(value,
                         -negamax(board, -beta, -alpha, depth - 1, opp, move));
            alpha = std::max(value, alpha);
            board.undo_move();
            if (alpha >= beta) {
                // Hash move to be prioritized in next search iteration
                best_move = move;

                // Update history heuristic
                if (move.flags & MoveFlag::Quiet) {
                    _history_heuristic[move.from.shift][move.to.shift] +=
                        (1 << depth);
                }
                break;
            }
        }

        // Update the transposition table
        TableEntry new_entry;
        new_entry.key = board.get_hash();
        new_entry.value = value;
        new_entry.depth = depth;
        new_entry.best_move = best_move;
        if (value <= alpha_orig) {
            new_entry.type = NodeType::Upper;
        } else if (value >= beta) {
            new_entry.type = NodeType::Lower;
        } else {
            new_entry.type = NodeType::Exact;
        }
        _transpositions.set(board, new_entry);
        return value;
    }

    float Search::negamax_root(Board &board, Move &move) {
        _start_time = std::chrono::steady_clock::now();
        float value = -INFINITY;
        float alpha = -3;
        float beta = 3;
        Color opp = static_cast<Color>(!board.get_turn());

        // Iterative deepening from an initial depth until it runs out of time
        board.execute_move(move);
        int depth = 1;
        while (depth <= _max_depth) {
            value =
                std::max(value, -negamax(board, alpha, beta, depth, opp, move));

            // Search timeout
            Time end_time = std::chrono::steady_clock::now();
            if ((end_time - _start_time).count() >= _iterative_timeout_ns) {
                break;
            }

            // Update aspiration window based on current depth
            bool re_search = false;
            if (value <= alpha) {
                alpha = std::max(-INFINITY, value - (10 << depth));
                re_search = true;
            }
            if (value >= beta) {
                beta = std::min(INFINITY, value + (10 << depth));
                re_search = true;
            }
            if (!re_search) {
                depth++;
            }
        }
        board.undo_move();

        return value;
    }

    float Search::ordering_heuristic(Board &board, const Move &move) {
        float score = 0;

        // Hashed move
        TableEntry &entry = _transpositions.get(board);
        if (entry.best_move == move) {
            score += 1000.0f;
        }

        // History heuristic
        if (move.flags & MoveFlag::Quiet) {
            score += _history_heuristic[move.from.shift][move.to.shift];
        }

        // Evaluate captures based on SEE heuristic
        if (move.flags & MoveFlag::Capture) {
            score += see_heuristic(board, move) * 10;
        }

        // Prioritize promotions
        if (move.flags & MoveFlag::QueenPromo) {
            score += piece_weights[PieceType::Queen];
        }
        if (move.flags & MoveFlag::BishopPromo) {
            score += piece_weights[PieceType::Bishop];
        }
        if (move.flags & MoveFlag::RookPromo) {
            score += piece_weights[PieceType::Rook];
        }
        if (move.flags & MoveFlag::KnightPromo) {
            score += piece_weights[PieceType::Knight];
        }

        return score;
    }

    float Search::evaluate(Board &board) {
        float material = board.get_material();
        float placement = placement_score(board);
        float mobility = mobility_score(board);

        return 2 * material + 0.1 * placement + 0.5 * mobility;
    }

    Move Search::move(Board &board) {
        Time start = std::chrono::steady_clock::now();

        std::vector<Move> moves = board.get_moves();
        Move best_move = moves[0];
        float best_value = -INFINITY;

        _visited = 0;
        for (auto &move : moves) {
            // Get the value making this move
            // Search will return the value from the opponent's POV
            float value = negamax_root(board, move);

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
} // namespace brainiac