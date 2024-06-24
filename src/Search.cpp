#include "Search.hpp"

namespace Brainiac {
    bool Search::can_reduce_move(Move move, MoveValue value) {
        MoveType type = move.type();
        switch (type) {
        case MoveType::Capture:
            return value < 40;
        case MoveType::QueenPromoCapture:
            return value < 50;
        case MoveType::Quiet:
            return true;
        default:
            return false;
        }
    }

    Value Search::negamax(Position &position,
                          Move prev,
                          Depth depth,
                          Value alpha,
                          Value beta,
                          bool qsearch) {
        // Time management
        _timeout = time() - _start_time >= _remaining_time;
        if (_timeout) return 0;

        _negamax_visited += !qsearch;
        _qsearch_visited += qsearch;

        // Read the transposition table
        Value alpha_orig = alpha;
        Node node = _tptable.get(position);
        if (node.type != NodeType::Invalid && node.depth >= depth &&
            node.hash == position.hash()) {
            switch (node.type) {
            case NodeType::Exact:
                return node.value;
            case NodeType::Lower:
                alpha = std::max(alpha, node.value);
                break;
            case NodeType::Upper:
                beta = std::min(beta, node.value);
                break;
            default:
                break;
            }

            // Early terminate
            if (alpha >= beta) {
                return node.value;
            }
        }

        // Check standing pat score
        if (qsearch && !position.is_check()) {
            Value stand_pat = evaluate(position);
            if (stand_pat >= beta) return stand_pat;
            if (stand_pat > alpha) alpha = stand_pat;
        }

        // Terminal node
        if (position.is_checkmate() || position.is_draw() ||
            (qsearch && (depth <= 0 || position.is_quiet()))) {
            return evaluate(position);
        } else if (!qsearch && depth <= 0) {
            return negamax(position,
                           prev,
                           MAX_QSEARCH_DEPTH,
                           alpha,
                           beta,
                           true);
        }

        // Null move reduction
        if (!position.is_check() && prev.type() != MoveType::Skip) {
            Depth R = depth > 6 ? 4 : 3;
            position.skip();
            Value score = -negamax(position,
                                   Move(),
                                   depth - R - 1,
                                   -beta,
                                   -beta + 1,
                                   qsearch);
            position.undo();
            if (score >= beta) {
                depth -= 4;
                if (depth <= 0) {
                    return evaluate(position);
                }
            }
        }

        // Compute moves to visit
        MovePicker picker;
        for (Move move : position.moves()) {
            switch (move.type()) {
            case MoveType::Capture:
            case MoveType::QueenPromoCapture:
            case MoveType::KnightPromoCapture:
            case MoveType::QueenPromo:
            case MoveType::KnightPromo:
                picker.add(move, position, _htable, node);
                break;
            default:
                if (!qsearch) {
                    picker.add(move, position, _htable, node);
                }
                break;
            }
        }

        // Non-terminal node
        Value value = MIN_VALUE;
        MoveEntry pick;
        while (!picker.end()) {
            pick = picker.next();

            // Skip bad captures in q-search
            if (qsearch && pick.value < 0 &&
                pick.phase == MovePhase::Captures) {
                continue;
            }

            // Compute depth reduction
            Depth R = (depth >= 3 && picker.search_index() > 3 &&
                       !position.is_check() &&
                       can_reduce_move(pick.move, pick.value));

            // Compute depth extension
            Depth E = depth < 2 && position.is_check();

            // Evaluate subtree
            position.make(pick.move);
            Value score = -negamax(position,
                                   pick.move,
                                   depth - R - 1 + E,
                                   -beta,
                                   -alpha,
                                   qsearch);
            if (R && score > alpha) {
                // Fail-low, do full re-search
                score = -negamax(position,
                                 pick.move,
                                 depth - 1 + E,
                                 -beta,
                                 -alpha,
                                 qsearch);
            }
            value = std::max(value, score);
            alpha = std::max(alpha, value);
            position.undo();

            // Early terminate
            if (alpha >= beta) {
                if (!_timeout && !qsearch) {
                    _htable.set(position, pick.move, depth);
                }
                break;
            }
        }

        // Update the transposition table
        if (!_timeout && !qsearch) {
            NodeType type = NodeType::Exact;
            if (value <= alpha_orig) {
                type = NodeType::Upper;
            } else if (value >= beta) {
                type = NodeType::Lower;
            }
            _tptable.set(position, type, depth, value, pick.move);
        }
        return value;
    }

    void Search::reset() {
        _tptable.clear();
        _htable.clear();
    }

    Result Search::search(Position &position) {
        // Reset statistics
        _timeout = false;
        _start_time = time();
        _negamax_visited = 0;
        _qsearch_visited = 0;
        _remaining_time = 15s; // TODO: Compute this dynamically.

        MoveList moves = position.moves();

        // Initial aspiration window
        Value alpha = MIN_VALUE;
        Value beta = MAX_VALUE;
        Value value = MIN_VALUE;

        Depth depth = 1;
        while (depth <= MAX_DEPTH && value != WIN_VALUE && !_timeout) {
            MoveIndex best_index = 0;
            for (MoveIndex i = 0; i < moves.size(); i++) {
                Move move = moves[i];

                // Evaluate the move
                position.make(move);
                Value score = -negamax(position, move, depth, alpha, beta);
                position.undo();

                // Check for cut-off
                if (score > value) {
                    best_index = i;
                    value = score;
                }
                if (value >= WIN_VALUE || _timeout) {
                    break;
                }
            }

            // Prioritize best_move in the next iteration
            if (!_timeout) {
                std::swap(moves[best_index], moves[0]);
            }
            depth++;
        }

        Result result;
        result.move = moves[0];
        result.time = time() - _start_time;
        result.visited = _negamax_visited + _qsearch_visited;
        result.max_depth = depth - 1;

        return result;
    }
} // namespace Brainiac