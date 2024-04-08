#include "Square.hpp"

namespace Brainiac {
    std::string square_to_string(Square sq) {
        assert(sq >= 0 && sq < 64);
        int row = sq / 8;
        int col = sq % 8;
        char rank = row + '1';
        char field = col + 'a';
        return std::string({field, rank});
    }

    Square string_to_square(std::string standard_notation) {
        int row = standard_notation[1] - '1';
        int col = standard_notation[0] - 'a';
        return Square(row * 8 + col);
    }
} // namespace Brainiac