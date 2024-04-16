#include "Perft.hpp"

namespace Brainiac {
    uint64_t perft(Game &game,
                   unsigned depth,
                   unsigned max_depth,
                   std::function<void(Move, uint64_t)> cb) {
        uint64_t nodes = 0;
        const MoveList &moves = game.moves();
        if (depth == 1) {
            return moves.size();
        }
        for (const Move &move : moves) {
            game.make(move);
            uint64_t children = perft(game, depth - 1, max_depth, cb);
            game.undo();
            if (depth == max_depth && cb) {
                cb(move, children);
            }
            nodes += children;
        }
        return nodes;
    }
} // namespace Brainiac