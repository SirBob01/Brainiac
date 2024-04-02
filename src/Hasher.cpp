#include "Hasher.hpp"

namespace Brainiac {
    Hasher::Hasher(unsigned seed) {
        std::default_random_engine rng{seed};
        std::uniform_real_distribution<double> uniform(0, UINT64_MAX);

        for (unsigned i = 0; i < BITSTRING_COUNT; i++) {
            _bitstrings[i] = uniform(rng);
        }
    }

    Hash Hasher::bitstring(Square square, Piece piece) const {
        return _bitstrings[square * 13 + piece];
    }

    Hash Hasher::bitstring(Square ep_dst) const {
        return _bitstrings[ep_dst * 13 + (Piece::BlackQueen + 1)];
    }

    Hash Hasher::bitstring(Color turn) const {
        return _bitstrings[64 * 13 + 1];
    }

    Hash Hasher::bitstring(CastlingRight right) const {
        return _bitstrings[64 * 13 + 1 + right];
    }

    Hash Hasher::operator()(Board &board,
                            CastlingFlagSet castling,
                            Color turn,
                            Square ep_dst) {
        Hash hash = 0;
        if (turn == Color::Black) {
            hash ^= bitstring(turn);
        }
        if (ep_dst != Square::Null) {
            hash ^= bitstring(ep_dst);
        }
        for (unsigned i = 0; i < 64; i++) {
            Square square = Square(i);
            Piece piece = board.get(square);
            if (piece != Piece::Empty) {
                hash ^= bitstring(square, piece);
            }
        }
        for (unsigned c = 0; c < 4; c++) {
            CastlingRight right = CastlingRight(c);
            if (castling & (1 << right)) {
                hash ^= bitstring(right);
            }
        }
        return hash;
    }
} // namespace Brainiac