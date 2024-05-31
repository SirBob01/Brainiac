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
        case QueenPromo: {
            unsigned i = index(position, move);
            MoveValue inc = depth * depth;

            // Protect from integer overflow
            if (MAX_MOVE_VALUE - _table[i] >= inc) {
                _table[i] += inc;
            } else {
                _table[i] = MAX_MOVE_VALUE;
            }
        } break;
        default:
            break;
        }
    }

    void History::clear() { std::fill(_table.begin(), _table.end(), 0); }
} // namespace Brainiac