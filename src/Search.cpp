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
        bool node_valid = node.type != NodeType::Invalid;
        bool node_move = node.move == move;
        if (node_valid && node_move) {
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
                          Value alpha,
                          Value beta) {
        // Time management
        _timeout = time() - _start_time >= _remaining_time;
        if (_timeout) return 0;
        _visited++;

        // Read the transposition table
        Value alpha_orig = alpha;
        Node node = _tptable.get(position);
        if (node.type != NodeType::Invalid && node.depth >= depth) {
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

        // Terminal node
        if (depth <= 0 || position.is_checkmate() || position.is_draw()) {
            return evaluate(position);
        }

        // Null move reduction
        if (!position.is_check() && prev.type() != MoveType::Skip) {
            Depth R = depth > 6 ? 4 : 3;
            position.skip();
            Value score =
                -negamax(position, Move(), depth - R - 1, -beta, -beta + 1);
            position.undo();
            if (score >= beta) {
                depth -= 4;
                if (depth <= 0) {
                    return evaluate(position);
                }
            }
        }

        // Non-terminal node
        Value value = MIN_VALUE;
        MoveList moves = position.moves();
        MoveValue best_value = 0;
        uint16_t best_index = 0;
        for (uint16_t i = 0; i < moves.size() && !_timeout; i++) {
            // Find highest scoring move
            best_index = i;
            best_value = evaluate_move(position, moves[best_index], node);
            for (uint16_t j = i + 1; j < moves.size(); j++) {
                MoveValue j_score = evaluate_move(position, moves[j], node);
                if (j_score > best_value) {
                    best_index = j;
                    best_value = j_score;
                }
            }
            Move &move = moves[best_index];

            // Compute depth reduction
            Depth R = (depth >= 3 && i > 3 && !position.is_check() &&
                       can_reduce_move(move, best_value));

            // Compute depth extension
            Depth E = depth < 2 && position.is_check();

            // Evaluate subtree
            position.make(move);
            Value score =
                -negamax(position, move, depth - R - 1 + E, -beta, -alpha);
            if (score > alpha) {
                // Fail-low, do full re-search
                score = -negamax(position, move, depth - 1 + E, -beta, -alpha);
            }
            value = std::max(value, score);
            alpha = std::max(alpha, value);
            position.undo();

            // Early terminate
            if (alpha >= beta && !_timeout) {
                _htable.set(position, move, depth);
                break;
            }

            // Swap with current index to sort
            std::swap(move, moves[i]);
        }

        // Update the transposition table
        node.depth = depth;
        node.value = value;
        node.move = moves[best_index];
        if (value <= alpha_orig) {
            node.type = NodeType::Upper;
        } else if (value >= beta) {
            node.type = NodeType::Lower;
        } else {
            node.type = NodeType::Exact;
        }
        if (!_timeout) _tptable.set(position, node);
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
        _visited = 0;
        _remaining_time = 15s; // TODO: Compute this dynamically.

        MoveList moves = position.moves();

        // Initial aspiration window
        Value alpha = MIN_VALUE;
        Value beta = MAX_VALUE;

        Depth depth = 1;
        Value best_value = MIN_VALUE;
        while (depth <= MAX_DEPTH && best_value != WIN_VALUE && !_timeout) {
            uint16_t best_index = 0;
            for (uint16_t i = 0; i < moves.size() && !_timeout; i++) {
                Move move = moves[i];

                // Evaluate the move
                position.make(move);
                Value score = -negamax(position, move, depth, alpha, beta);
                if (score > best_value) {
                    best_value = score;
                    best_index = i;
                }
                position.undo();
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
        result.visited = _visited;
        result.max_depth = depth - 1;
        return result;
    }
} // namespace Brainiac