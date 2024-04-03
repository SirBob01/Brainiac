#include "Bitboard.hpp"

namespace Brainiac {
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