#include "Search.hpp"

namespace Brainiac {
    int Search::score_move(Move move, Node node) {
        int score = 0;

        // Prioritize hash moves
        bool node_valid = node.type != NodeType::Invalid;
        bool node_move = node.move == move;
        score += (node_move && node_valid) * 100000000;

        // Prioritize moves with higher history heuristic
        score += _htable.get(move);

        // Prioritize non-quiet moves
        switch (move.type()) {
        case MoveType::Capture:
        case MoveType::EnPassant:
            score += 20;
            break;
        case MoveType::KnightPromo:
        case MoveType::BishopPromo:
        case MoveType::RookPromo:
        case MoveType::QueenPromo:
            score += 10;
            break;
        case MoveType::KnightPromoCapture:
        case MoveType::BishopPromoCapture:
        case MoveType::RookPromoCapture:
        case MoveType::QueenPromoCapture:
            score += 30;
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
                int score_j = score_move(moves[j], node);
                int score_i = score_move(moves[move_index], node);
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