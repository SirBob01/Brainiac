#include "Bitboard.hpp"

namespace Brainiac {
    unsigned find_lsb_bitboard(Bitboard bitboard) {
        return __builtin_ctzll(bitboard);
    }

    unsigned count_set_bitboard(Bitboard bitboard) {
        return __builtin_popcountll(bitboard);
    }

    Bitboard flip_vertical_bitboard(Bitboard bitboard) {
        return __builtin_bswap64(bitboard);
    }

    Bitboard flip_horizontal_bitboard(Bitboard bitboard) {
        constexpr Bitboard hk_0 = 0x5555555555555555;
        constexpr Bitboard hk_1 = 0x3333333333333333;
        constexpr Bitboard hk_2 = 0x0f0f0f0f0f0f0f0f;
        bitboard = ((bitboard >> 1) & hk_0) + 2 * (bitboard & hk_0);
        bitboard = ((bitboard >> 2) & hk_1) + 4 * (bitboard & hk_1);
        bitboard = ((bitboard >> 4) & hk_2) + 16 * (bitboard & hk_2);
        return bitboard;
    }

    Bitboard get_lsb_bitboard(Bitboard bitboard) {
        return bitboard & (-bitboard);
    }

    Bitboard pop_lsb_bitboard(Bitboard bitboard) {
        return bitboard & (bitboard - 1);
    }

    void print_bitboard(Bitboard bitboard) {
        std::bitset<64> bitarray(bitboard);
        std::stack<std::string> bytes;

        std::string s = "";
        for (int i = 0; i < 64; i++) {
            s += (bitarray[i] ? "o " : ". ");
            if ((i + 1) % 8 == 0) {
                s += "\n";
                bytes.push(s);
                s = "";
            }
        }
        if (s.length()) bytes.push(s);
        while (bytes.size()) {
            std::cout << bytes.top();
            bytes.pop();
        }
        std::cout << "\n";
    }
} // namespace Brainiac