#include "bits.h"

namespace brainiac {
    SlidingMoveTable rook_attack_tables[64] = {};
    SlidingMoveTable bishop_attack_tables[64] = {};
    uint64_t king_move_masks[64] = {};
    uint64_t knight_move_masks[64] = {};

    void print_bitboard(uint64_t bitboard) {
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
} // namespace brainiac