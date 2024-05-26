#include "Search.hpp"

namespace Brainiac {
    Value Search::see_target(Square target) {
        // TODO: Too slow
        const Board &board = _position.board();
        const MoveList &moves = _position.moves();

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
            _position.make(best_move);
            value = std::max(0, mvv - see_target(target));
            _position.undo();
        }

        return value;
    }

    MoveValue Search::evaluate_capture(Move move) {
        const Board &board = _position.board();
        Piece attacker = board.get(move.src());
        Piece victim = board.get(move.dst());
        return std::abs(PIECE_WEIGHTS[victim]) -
               std::abs(PIECE_WEIGHTS[attacker]);
    }

    MoveValue Search::evaluate_move(Move move, Node node) {
        // Prioritize hash moves
        bool node_valid = node.type != NodeType::Invalid;
        bool node_move = node.move == move;
        if (node_valid && node_move) {
            return MAX_MOVE_VALUE;
        }

        // Prioritize moves with higher history heuristic
        MoveValue value = _htable.get(move);

        // Prioritize non-quiet moves
        // Also, prioritize queen promotion over all other promotions
        switch (move.type()) {
        case MoveType::Capture:
            value += 40 + evaluate_capture(move);
            break;
        case MoveType::QueenPromoCapture:
            value += 50 + evaluate_capture(move);
            break;
        case MoveType::EnPassant:
            value += 20;
            break;
        case MoveType::QueenPromo:
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

    Value Search::negamax(Depth depth, Value alpha, Value beta) {
        _visited++;

        // Read the transposition table
        Value alpha_orig = alpha;
        Node node = _tptable.get(_position);
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
        if (depth == 0 || _position.is_checkmate() || _position.is_draw()) {
            return evaluate(_position);
        }

        // Non-terminal node
        Value value = MIN_VALUE;
        MoveList moves = _position.moves();
        MoveValue best_value = 0;
        uint16_t best_index = 0;
        for (uint16_t i = 0; i < moves.size(); i++) {
            // Find highest scoring move
            best_index = i;
            best_value = evaluate_move(moves[best_index], node);
            for (uint16_t j = i + 1; j < moves.size(); j++) {
                MoveValue j_score = evaluate_move(moves[j], node);
                if (j_score > best_value) {
                    best_index = j;
                    best_value = j_score;
                }
            }
            Move &move = moves[best_index];

            // Compute depth reduction
            Depth R = (depth >= 3 && i > 3 && !_position.is_check() &&
                       can_reduce_move(move, best_value));

            // Evaluate subtree
            _position.make(move);
            Value score = -negamax(depth - R - 1, -beta, -alpha);
            value = std::max(value, score);
            alpha = std::max(alpha, value);
            _position.undo();

            // Early terminate
            if (alpha >= beta) {
                _htable.set(move, depth);
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
        _tptable.set(_position, node);
        return value;
    }

    Result Search::search() {
        // Reset statistics
        _start_time = time();
        _visited = 0;

        MoveList moves = _position.moves();
        for (Depth d = 1; d <= MAX_DEPTH; d++) {
            Value best_value = MIN_VALUE;
            uint16_t best_index = 0;

            for (uint16_t i = 0; i < moves.size(); i++) {
                Move move = moves[i];

                _position.make(move);

                // Evaluate the move
                Value score = -negamax(d);
                if (score > best_value || best_value == MIN_VALUE) {
                    best_value = score;
                    best_index = i;
                }

                _position.undo();
            }

            // Prioritize best_move in the next iteration
            std::swap(moves[best_index], moves[0]);
        }

        Result result;
        result.move = moves[0];
        result.time = time() - _start_time;
        result.visited = _visited;
        return result;
    }
} // namespace Brainiac