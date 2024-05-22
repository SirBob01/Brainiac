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
        return _bitstrings[square * 12 + piece];
    }

    Hash Hasher::bitstring(Square ep_dst) const {
        return _bitstrings[64 * 12 + ep_dst];
    }

    Hash Hasher::bitstring(Color turn) const {
        return _bitstrings[64 * 12 + 64];
    }

    Hash Hasher::bitstring(CastlingFlagSet castling) const {
        return _bitstrings[64 * 12 + 64 + 1 + castling];
    }

    Hash Hasher::operator()(Board &board,
                            CastlingFlagSet castling,
                            Color turn,
                            Square ep_dst) const {
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
        hash ^= bitstring(castling);
        return hash;
    }
} // namespace Brainiac