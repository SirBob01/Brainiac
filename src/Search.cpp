#include "Search.hpp"

namespace Brainiac {
    Value Search::see_target(Position &position, Square target) {
        const Board &board = position.board();
        const MoveList &moves = position.moves();

        Piece victim = board.get(target);
        if (victim == Piece::Empty) {
            return 0;
        }

        Value value = 0;
        Value mvv = std::abs(PIECE_WEIGHTS[victim]);
        Value lva = MAX_VALUE;

        Move best_move = moves[0];
        for (Move move : moves) {
            if (move.dst() != target) continue;

            Piece attacker = board.get(move.src());
            Value attacker_value = std::abs(PIECE_WEIGHTS[attacker]);
            if (attacker_value < lva) {
                lva = attacker_value;
                best_move = move;
            }
        }

        if (lva != MAX_VALUE) {
            position.make(best_move);
            value = std::max(0, mvv - see_target(position, target));
            position.undo();
        }

        return value;
    }

    MoveValue Search::evaluate_capture(Position &position, Move move) {
        const Board &board = position.board();
        Piece victim = board.get(move.dst());

        position.make(move);
        Value value =
            std::abs(PIECE_WEIGHTS[victim]) - see_target(position, move.dst());
        position.undo();
        return value;
    }

    MoveValue Search::evaluate_move(Position &position, Move move, Node node) {
        // Prioritize hash moves
        bool node_type = node.type != NodeType::Invalid;
        bool node_move = node.move == move;
        bool node_hash = node.hash == position.hash();
        if (node_type && node_move && node_hash) {
            return MAX_MOVE_VALUE;
        }

        // Prioritize moves with higher history heuristic
        MoveValue value = _htable.get(position, move);

        // Prioritize non-quiet moves
        // Also, prioritize queen and knight promotions over all others
        switch (move.type()) {
        case MoveType::Capture:
            value += 40 + evaluate_capture(position, move);
            break;
        case MoveType::QueenPromoCapture:
        case MoveType::KnightPromoCapture:
            value += 50 + evaluate_capture(position, move);
            break;
        case MoveType::EnPassant:
            value += 20;
            break;
        case MoveType::QueenPromo:
        case MoveType::KnightPromo:
            value += 20;
            break;
        case MoveType::KingCastle:
        case MoveType::QueenCastle:
            value += 10;
            break;
        case MoveType::PawnDouble:
            value += 10;
            break;
        default:
            break;
        }
        return value;
    }

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
                          Depth ply,
                          Value alpha,
                          Value beta,
                          bool qsearch) {
        // Clear the PV at this ply
        _pvtable.clear(ply);

        // Time management
        Seconds elapsed = time() - _start_time;
        _timeout = elapsed >= _limit_time && _limit_time.count() >= 0;
        if (!_running || _timeout) return 0;

        // Update visited statistics
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
                           ply + 1,
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
                                   ply + 1,
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
        MoveList moves;
        for (Move move : position.moves()) {
            switch (move.type()) {
            case MoveType::Capture:
            case MoveType::QueenPromoCapture:
            case MoveType::KnightPromoCapture:
            case MoveType::QueenPromo:
            case MoveType::KnightPromo:
                moves.add(move);
                break;
            default:
                if (!qsearch) {
                    moves.add(move);
                }
                break;
            }
        }

        // Non-terminal node
        Value value = MIN_VALUE;
        MoveValue move_value = 0;
        MoveIndex move_index = 0;
        for (MoveIndex i = 0; i < moves.size(); i++) {
            // Find highest scoring move
            move_index = i;
            move_value = evaluate_move(position, moves[move_index], node);
            for (MoveIndex j = i + 1; j < moves.size(); j++) {
                MoveValue j_value = evaluate_move(position, moves[j], node);
                if (j_value > move_value) {
                    move_index = j;
                    move_value = j_value;
                }
            }
            Move &move = moves[move_index];

            // Skip bad captures
            if (qsearch) {
                switch (move.type()) {
                case MoveType::Capture:
                    if (move_value < 40) continue;
                    break;
                case MoveType::QueenPromoCapture:
                case MoveType::KnightPromoCapture:
                    if (move_value < 50) continue;
                    break;
                default:
                    break;
                }
            }

            // Compute depth reduction
            Depth R = (depth >= 3 && i > 3 && !position.is_check() &&
                       can_reduce_move(move, move_value));

            // Compute depth extension
            Depth E = depth < 2 && position.is_check();

            // Evaluate subtree
            position.make(move);
            Value score = -negamax(position,
                                   move,
                                   depth - R - 1 + E,
                                   ply + 1,
                                   -beta,
                                   -alpha,
                                   qsearch);
            if (R && score > alpha) {
                // Fail-low, do full re-search
                score = -negamax(position,
                                 move,
                                 depth - 1 + E,
                                 ply + 1,
                                 -beta,
                                 -alpha,
                                 qsearch);
            }
            value = std::max(value, score);
            alpha = std::max(alpha, value);
            position.undo();

            // Early terminate
            if (alpha >= beta) {
                if (_running && !_timeout && !qsearch) {
                    _htable.set(position, move, depth);
                    _pvtable.update(ply, move);
                }
                break;
            }

            // Swap with current index to sort
            std::swap(move, moves[i]);
        }

        // Update the transposition table
        if (_running && !_timeout && !qsearch) {
            NodeType type = NodeType::Exact;
            if (value <= alpha_orig) {
                type = NodeType::Upper;
            } else if (value >= beta) {
                type = NodeType::Lower;
            } else {
                _pvtable.update(ply, moves[move_index]);
            }
            _tptable.set(position, type, depth, value, moves[move_index]);
        }
        return value;
    }

    void Search::reset() {
        _tptable.clear();
        _htable.clear();
    }

    void Search::set_iterative_callback(IterativeCallback callback) {
        _on_iterative = callback;
    };

    void Search::set_pv_callback(PVCallback callback) { _on_pv = callback; };

    void Search::set_bestmove_callback(BestMoveCallback callback) {
        _on_bestmove = callback;
    }

    void Search::go(Position &position, SearchLimits limits) {
        // A search is currently running
        if (_running) return;
        _running = true;

        // Reset timer
        _timeout = false;
        _start_time = time();
        if (limits.move_time.count()) {
            _limit_time = limits.move_time;
        } else {
            // Compute time per move
            Seconds increment = position.turn() == Color::White
                                    ? limits.white_increment
                                    : limits.black_increment;
            _limit_time = position.turn() == Color::White ? limits.white_time
                                                          : limits.black_time;
            if (limits.moves_to_go) {
                _limit_time /= limits.moves_to_go;
            } else {
                _limit_time /= 30;
            }

            // Apply increment
            if (_limit_time >= increment) {
                _limit_time += increment;
            }

            // Assume infinite time if no time was provided
            if (!_limit_time.count()) {
                _limit_time = Seconds(-1);
            }
        }

        _negamax_visited = 0;
        _qsearch_visited = 0;

        IterativeInfo iterative_info;
        MoveList moves = position.moves();

        // Initial aspiration window
        Value alpha = MIN_VALUE;
        Value beta = MAX_VALUE;
        Value value = MIN_VALUE;

        Depth depth = 1;
        while (depth <= limits.depth && value != WIN_VALUE) {
            MoveIndex best_index = 0;
            for (MoveIndex i = 0; i < moves.size(); i++) {
                Move move = moves[i];

                // Clear PV at this ply
                _pvtable.clear(0);

                // Evaluate the move
                position.make(move);
                Value score = -negamax(position, move, depth, 1, alpha, beta);
                position.undo();

                // Check for cut-off
                if (score > value) {
                    best_index = i;
                    value = score;

                    // Update the PV
                    _pvtable.update(0, move);

                    PVInfo info;
                    info.depth = depth;
                    info.time = time() - _start_time;
                    info.nodes = _negamax_visited + _qsearch_visited;
                    info.value = value;
                    info.pv_length = _pvtable.get_length(0);
                    for (unsigned i = 0; i < info.pv_length; i++) {
                        info.pv[i] = _pvtable.get(0, i);
                    }
                    _on_pv(info);
                }

                // Traversal callback
                iterative_info.move = move;
                iterative_info.move_number = i + 1;
                iterative_info.depth = depth;
                _on_iterative(iterative_info);

                if (value >= WIN_VALUE || !_running || _timeout) {
                    break;
                }
            }

            // Prioritize best_move in the next iteration
            if (_running && !_timeout) {
                std::swap(moves[best_index], moves[0]);
            } else {
                break;
            }

            depth++;
        }

        _on_bestmove(moves[0]);
        _running = false;
    }

    void Search::stop() { _running = false; }
} // namespace Brainiac