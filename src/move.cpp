#include "move.h"

namespace brainiac {
    std::string square_to_string(Square sq) {
        assert(sq >= 0 && sq < 64);
        int row = sq / 8;
        int col = sq % 8;
        char rank = row + '1';
        char field = col + 'a';
        return std::string({field, rank});
    }

    Square string_to_square(std::string notation) {
        int row = notation[1] - '1';
        int col = notation[0] - 'a';
        return row * 8 + col;
    }
} // namespace brainiac