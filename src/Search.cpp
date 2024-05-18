#include "Search.hpp"

namespace Brainiac {
    short Search::negamax(Position &pos, short depth, short alpha, short beta) {
        // Read the transposition table
        short alpha_orig = alpha;
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

        // Terminal node, evaluate
        if (depth == 0 || pos.is_checkmate() || pos.is_draw()) {
            return evaluate(pos);
        }

        // Evaluate subtrees
        short value = LOWER_BOUND;
        for (Move move : pos.moves()) {
            pos.make(move);
            short score = -negamax(pos, depth - 1, -beta, -alpha);
            value = std::max(value, score);
            alpha = std::max(alpha, value);
            pos.undo();

            // Early terminate
            if (alpha >= beta) {
                break;
            }
        }

        // Update the transposition table
        TableEntry new_entry;
        new_entry.depth = depth;
        new_entry.value = value;
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
        Move best_move;
        short best_score = LOWER_BOUND;
        for (Move move : pos.moves()) {
            pos.make(move);

            // Evaluate the move
            short score = -negamax(pos, MAX_DEPTH);
            if (score > best_score || best_score == LOWER_BOUND) {
                best_score = score;
                best_move = move;
            }

            pos.undo();
        }
        return best_move;
    }
} // namespace Brainiac