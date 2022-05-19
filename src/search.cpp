#include "search.h"

namespace brainiac {
    Search::Search() {
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
            return entry.value; // Entry is invalid, this will return -INFINITY
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
        } else if ((depth == -MAX_QUIESCENCE_DEPTH) ||
                   (depth <= 0 && (move.flags & MoveFlag::Quiet))) {
            float score = evaluate(board);
            return turn == Color::White ? score : -score;
        }

        Color opp = static_cast<Color>(!turn);
        float value = -INFINITY;
        Move best_move;

        // Null move reduction
        if (depth >= 5 && !board.is_check()) {
            int R = depth > 6 ? 4 : 3;
            board.skip_turn();
            float reduction =
                -negamax(board, -beta, -beta + 1, depth - R - 1, opp, move);
            board.undo_move();
            if (reduction >= beta) {
                depth -= 4;
            }
        }

        // Get move list
        const std::vector<Move> &moves = board.get_moves();
        int n = moves.size();

        // Map moves to heuristic scores for move ordering
        std::vector<MoveScore> move_scores;
        for (const Move &move : moves) {
            float score = ordering_heuristic(board, move, depth);
            move_scores.emplace_back(move, score);
        }

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

            // Prune only after a PV move has been found
            if (value != -INFINITY) {
                // Prune bad captures during quiescence search
                if (depth < 0 && move_scores[i].score < 0) {
                    continue;
                }

                // PV search with late move reduction
                // Tactical moves should not be reduced
                if (i > 1 && depth > 3 && !board.is_check()) {
                    // Adjust reduction value
                    int R = 0;
                    if (!(move.flags & LMR_MOVE_FILTER)) {
                        R++;
                        if (i > 6) {
                            R++;
                        }
                    }

                    // Search with reduced depth and null-window
                    board.execute_move(move);
                    float reduction = -negamax(board,
                                               -alpha - 1,
                                               -alpha,
                                               depth - R - 1,
                                               opp,
                                               move);
                    board.undo_move();

                    // This move is proven to be not good, skip it
                    if (reduction <= alpha || reduction >= beta) {
                        continue;
                    }
                }
            }

            // Full search negamax
            board.execute_move(move);
            float search = -negamax(board, -beta, -alpha, depth - 1, opp, move);
            board.undo_move();

            // Ignore timedout search
            if (search == INFINITY) {
                return -INFINITY;
            }

            if (search >= value) {
                value = search;
                best_move = move;
                if (value >= alpha) {
                    alpha = value;
                }
                if (alpha >= beta) {
                    // Update history heuristic and killer move
                    if (move.flags & MoveFlag::Quiet) {
                        int from = move.from.shift;
                        int to = move.to.shift;
                        _history_heuristic[from][to] += (1 << depth);
                    }
                    if (depth >= 0) {
                        _killer_moves[depth] = move;
                    }
                    break;
                }
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
        float alpha = -INFINITY;
        float beta = INFINITY;
        Color opp = static_cast<Color>(!board.get_turn());

        // Iterative deepening from an initial depth until it runs out of time
        board.execute_move(move);
        int depth = 1;
        while (depth <= MAX_DEPTH) {
            float search = -negamax(board, alpha, beta, depth, opp, move);
            if (search != INFINITY) {
                value = search;
            }

            // Search timeout
            Time end_time = std::chrono::steady_clock::now();
            if ((end_time - _start_time).count() >= _iterative_timeout_ns) {
                break;
            }
            depth++;
        }
        board.undo_move();

        return value;
    }

    float
    Search::ordering_heuristic(Board &board, const Move &move, int depth) {
        float score = 0;

        // Hashed move
        TableEntry &entry = _transpositions.get(board);
        if (entry.best_move == move) {
            score += 100000.0f;
        }

        // Killer move
        if (depth >= 0 && _killer_moves[depth] == move) {
            score += 1000.0f;
        }

        // History heuristic
        if (move.flags & MoveFlag::Quiet) {
            score += _history_heuristic[move.from.shift][move.to.shift];
        }

        // Evaluate captures based on SEE heuristic
        if (move.flags & MoveFlag::Capture) {
            score += see_heuristic(board, move) * 100;
        }

        // Prioritize promotions
        if (move.flags & MoveFlag::QueenPromo) {
            score += piece_weights[PieceType::Queen] * 10;
        }
        if (move.flags & MoveFlag::BishopPromo) {
            score += piece_weights[PieceType::Bishop] * 10;
        }
        if (move.flags & MoveFlag::RookPromo) {
            score += piece_weights[PieceType::Rook] * 10;
        }
        if (move.flags & MoveFlag::KnightPromo) {
            score += piece_weights[PieceType::Knight] * 10;
        }

        return score;
    }

    float Search::evaluate(Board &board) {
        float material = board.get_material();
        float placement = placement_score(board);
        float mobility = mobility_score(board);
        float pawn_connected = connected_pawn_score(board);
        float bishop_pair = bishop_pair_score(board);

        return 2 * material + 0.2 * placement + 0.5 * mobility +
               0.25 * pawn_connected + 1.5 * bishop_pair;
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
        std::cout << best_move.standard_notation() << " | " << best_value
                  << "\n";
        return best_move;
    }

    std::vector<Move> Search::get_principal_variation(Board &board) {
        std::vector<Move> pv;

        // Search for PV moves from the transposition table
        int count = 0;
        for (int ply = 0; ply < MAX_DEPTH; ply++) {
            TableEntry &entry = _transpositions.get(board);
            if (!entry.best_move.is_invalid()) {
                count++;
                pv.push_back(entry.best_move);
                board.execute_move(entry.best_move);
            } else {
                break;
            }
        }

        // Move board back to initial position
        while (count) {
            board.undo_move();
            count--;
        }
        return pv;
    }
} // namespace brainiac