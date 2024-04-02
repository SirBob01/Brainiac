#include "Hasher.hpp"

namespace Brainiac {
    Hasher::Hasher(unsigned seed) {
        std::default_random_engine rng{seed};
        std::uniform_real_distribution<double> uniform(0, UINT64_MAX);

        for (unsigned i = 0; i < BITSTRING_COUNT; i++) {
            bitstrings.push_back(uniform(rng));
        }
    }

    Hash Hasher::hash(Board &board,
                      CastlingFlagSet castling,
                      Color turn,
                      Square ep_target) {
        Hash hash = 0;
        if (turn == Color::Black) {
            hash ^= bitstrings[BITSTRING_COUNT - 1];
        }
        if (ep_target != Square::Null) {
            hash ^= bitstrings[BITSTRING_COUNT - 2];
        }
        for (uint8_t c = 0; c < 4; c++) {
            if (castling & (1 << c)) {
                hash ^= bitstrings[BITSTRING_COUNT - 3 - c];
            }
        }
        for (uint8_t sq = 0; sq < 64; sq++) {
            Piece piece = board.get(Square(sq));
            if (piece != Piece::Empty) {
                hash ^= bitstrings[sq * 12 + piece];
            }
        }
        return hash;
    }
} // namespace Brainiac