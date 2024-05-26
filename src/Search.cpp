#include "Search.hpp"

namespace Brainiac {
    Value Search::see_target(Square target, Position &pos) {
        const Board &board = pos.board();
        const MoveList &moves = pos.moves();

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
            pos.make(best_move);
            value = std::max(0, mvv - see_target(target, pos));
            pos.undo();
        }

        return value;
    }

    MoveValue Search::see_capture(Move move, Position &pos) {
        Piece attacker = pos.board().get(move.src());
        MoveValue value = std::abs(PIECE_WEIGHTS[attacker]);

        pos.make(move);
        value -= see_target(move.dst(), pos);
        pos.undo();

        return value;
    }

    MoveValue Search::score_move(Move move, Node node, Position &pos) {
        // Prioritize hash moves
        bool node_valid = node.type != NodeType::Invalid;
        bool node_move = node.move == move;
        if (node_valid && node_move) {
            return MAX_MOVE_VALUE;
        }

        // Prioritize moves with higher history heuristic
        MoveValue score = _htable.get(move);

        // Prioritize non-quiet moves
        // Also, prioritize queen promotion over all other promotions
        switch (move.type()) {
        case MoveType::Capture:
            score += 20 + see_capture(move, pos);
            break;
        case MoveType::QueenPromoCapture:
            score += 30 + see_capture(move, pos);
            break;
        case MoveType::EnPassant:
            score += 20;
            break;
        case MoveType::QueenPromo:
            score += 10;
            break;
        case MoveType::KingCastle:
        case MoveType::QueenCastle:
            score += 10;
            break;
        case MoveType::PawnDouble:
            score += 5;
            break;
        default:
            break;
        }
        return score;
    }

    Value Search::negamax(Position &pos, Depth depth, Value alpha, Value beta) {
        // Update statistics
        _result.visited++;

        // Read the transposition table
        Value alpha_orig = alpha;
        Node node = _tptable.get(pos);
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
        if (depth == 0 || pos.is_checkmate() || pos.is_draw()) {
            return evaluate(pos);
        }

        // Non-terminal node
        Value value = MIN_VALUE;
        MoveList moves = pos.moves();
        unsigned move_index = 0;
        for (unsigned i = 0; i < moves.size(); i++) {
            // Find highest scoring move
            move_index = i;
            for (unsigned j = i + 1; j < moves.size(); j++) {
                MoveValue score_j = score_move(moves[j], node, pos);
                MoveValue score_i = score_move(moves[move_index], node, pos);
                if (score_j > score_i) {
                    move_index = j;
                }
            }
            Move &move = moves[move_index];

            // Evaluate subtree
            pos.make(move);
            Value score = -negamax(pos, depth - 1, -beta, -alpha);
            value = std::max(value, score);
            alpha = std::max(alpha, value);
            pos.undo();

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
        node.move = moves[move_index];
        if (value <= alpha_orig) {
            node.type = NodeType::Upper;
        } else if (value >= beta) {
            node.type = NodeType::Lower;
        } else {
            node.type = NodeType::Exact;
        }
        _tptable.set(pos, node);
        return value;
    }

    Result Search::search(Position &pos) {
        // Reset result
        _result.time = time();
        _result.visited = 0;

        MoveList moves = pos.moves();
        for (Depth d = 1; d <= MAX_DEPTH; d++) {
            Value best_score = MIN_VALUE;
            unsigned best_move = 0;

            for (unsigned i = 0; i < moves.size(); i++) {
                Move move = moves[i];

                pos.make(move);

                // Evaluate the move
                Value score = -negamax(pos, d);
                if (score > best_score || best_score == MIN_VALUE) {
                    best_score = score;
                    best_move = i;
                }

                pos.undo();
            }

            // Prioritize best_move in the next iteration
            std::swap(moves[best_move], moves[0]);
        }

        // Update result
        _result.move = moves[0];
        _result.time = time() - _result.time;

        return _result;
    }
} // namespace Brainiac