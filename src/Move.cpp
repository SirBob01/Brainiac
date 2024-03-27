#include "Move.hpp"

namespace Brainiac {
    Bitboard get_square_mask(Square sq) { return 1ULL << sq; };

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

    Move::Move(Square from, Square to, MoveType flags) :
        _bitfield(to | (from << 6) | (flags << 12)){};
    Move::Move() : _bitfield(0){};

    Square Move::from() const { return Square((_bitfield >> 6) & 0x3F); }

    Square Move::to() const { return Square(_bitfield & 0x3F); }

    MoveType Move::type() const { return MoveType(_bitfield >> 12); }

    bool Move::operator==(const Move &other) const {
        return _bitfield == other._bitfield;
    }

    std::string Move::standard_notation() const {
        std::string s = square_to_string(from()) + square_to_string(to());
        MoveType move_type = type();
        if (move_type == MoveType::RookPromo ||
            move_type == MoveType::RookPromoCapture)
            s += 'r';
        if (move_type == MoveType::BishopPromo ||
            move_type == MoveType::BishopPromoCapture)
            s += 'b';
        if (move_type == MoveType::KnightPromo ||
            move_type == MoveType::KnightPromoCapture)
            s += 'n';
        if (move_type == MoveType::QueenPromo ||
            move_type == MoveType::QueenPromoCapture)
            s += 'q';
        return s;
    };
} // namespace Brainiac