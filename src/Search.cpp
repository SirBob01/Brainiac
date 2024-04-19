#include "Search.hpp"

namespace Brainiac {
    short Search::negamax(Position &pos, short depth, short alpha, short beta) {
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