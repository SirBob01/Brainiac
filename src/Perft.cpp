#include "Perft.hpp"

namespace Brainiac {
    uint64_t perft(Position &pos,
                   Depth depth,
                   Depth max_depth,
                   std::function<void(Move, uint64_t)> cb) {
        uint64_t nodes = 0;
        MoveList moves = pos.moves();
        if (depth == 1) {
            return moves.size();
        }
        for (Move move : moves) {
            pos.make(move);
            uint64_t children = perft(pos, depth - 1, max_depth, cb);
            pos.undo();
            if (depth == max_depth && cb) {
                cb(move, children);
            }
            nodes += children;
        }
        return nodes;
    }
} // namespace Brainiac