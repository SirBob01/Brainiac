#include "Bitboard.hpp"

namespace Brainiac {
    unsigned count_set_bitboard(Bitboard bitboard) {
        unsigned count = 0;
        while (bitboard) {
            bitboard = pop_lsb_bitboard(bitboard);
            count++;
        }
        return count;
    }

    Bitboard pop_lsb_bitboard(Bitboard bitboard) {
        return bitboard & (bitboard - 1);
    }
} // namespace Brainiac