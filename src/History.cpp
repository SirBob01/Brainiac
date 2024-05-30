#include "History.hpp"

namespace Brainiac {
    History::History() { clear(); }

    unsigned History::index(const Position &position, Move move) const {
        const Board &board = position.board();
        Piece piece = board.get(move.src());
        return piece * 64 + move.dst();
    }

    MoveValue History::get(const Position &position, Move move) const {
        return _table[index(position, move)];
    }

    void History::set(const Position &position, Move move, Depth depth) {
        switch (move.type()) {
        case Quiet:
        case PawnDouble:
        case KingCastle:
        case QueenCastle:
        case KnightPromo:
        case RookPromo:
        case BishopPromo:
        case QueenPromo:
            _table[index(position, move)] += depth * depth;
            break;
        default:
            break;
        }
    }

    void History::clear() { std::fill(_table.begin(), _table.end(), 0); }
} // namespace Brainiac