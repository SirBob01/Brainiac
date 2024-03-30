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