#include "History.hpp"

namespace Brainiac {
    unsigned History::index(Move move) const {
        return move.src() * 64 + move.dst();
    }

    int History::get(Move move) const { return _table[index(move)]; }

    void History::set(Move move, Depth depth) {
        switch (move.type()) {
        case Quiet:
        case PawnDouble:
        case KingCastle:
        case QueenCastle:
        case KnightPromo:
        case RookPromo:
        case BishopPromo:
        case QueenPromo:
            _table[index(move)] += depth * depth;
            break;
        default:
            break;
        }
    }
} // namespace Brainiac