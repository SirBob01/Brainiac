#include "Search.hpp"

namespace Brainiac {
    int Search::score_move(Move move, TableEntry node) {
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

    int Search::negamax(Position &pos, unsigned depth, int alpha, int beta) {
        // Read the transposition table
        int alpha_orig = alpha;
        TableEntry entry = _tptable.get(pos);
        if (entry.type != NodeType::Invalid && entry.depth >= depth) {
            switch (entry.type) {
            case NodeType::Exact:
                return entry.value;
            case NodeType::Lower:
                alpha = std::max(alpha, entry.value);
                break;
            case NodeType::Upper:
                beta = std::min(beta, entry.value);
                break;
            default:
                break;
            }

            // Early terminate
            if (alpha >= beta) {
                return entry.value;
            }
        }

        // Terminal node
        if (depth == 0 || pos.is_checkmate() || pos.is_draw()) {
            return evaluate(pos);
        }

        // Non-terminal node
        int value = LOWER_BOUND;
        MoveList moves = pos.moves();
        unsigned move_index = 0;
        for (unsigned i = 0; i < moves.size(); i++) {
            // Find highest scoring move
            move_index = i;
            for (unsigned j = i + 1; j < moves.size(); j++) {
                int score_j = score_move(moves[j], entry);
                int score_i = score_move(moves[move_index], entry);
                if (score_j > score_i) {
                    move_index = j;
                }
            }
            Move &move = moves[move_index];

            // Evaluate subtree
            pos.make(move);
            int score = -negamax(pos, depth - 1, -beta, -alpha);
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
        TableEntry new_entry;
        new_entry.depth = depth;
        new_entry.value = value;
        new_entry.move = moves[move_index];
        if (value <= alpha_orig) {
            new_entry.type = NodeType::Upper;
        } else if (value >= beta) {
            new_entry.type = NodeType::Lower;
        } else {
            new_entry.type = NodeType::Exact;
        }
        _tptable.set(pos, new_entry);
        return value;
    }

    Move Search::move(Position &pos) {
        MoveList moves = pos.moves();
        for (unsigned d = 1; d <= MAX_DEPTH; d++) {
            int best_score = LOWER_BOUND;
            int best_move = 0;

            for (unsigned i = 0; i < moves.size(); i++) {
                Move move = moves[i];

                pos.make(move);

                // Evaluate the move
                int score = -negamax(pos, d);
                if (score > best_score || best_score == LOWER_BOUND) {
                    best_score = score;
                    best_move = i;
                }

                pos.undo();
            }

            // Prioritize best_move in the next iteration
            std::swap(moves[best_move], moves[0]);
        }

        return moves[0];
    }
} // namespace Brainiac