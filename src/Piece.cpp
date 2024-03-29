#include "Piece.hpp"

namespace Brainiac {
    Piece::Piece(PieceType type, Color color) : _bitfield(type << 1 | color){};

    Piece::Piece() : _bitfield(0){};

    PieceType Piece::type() const { return PieceType(_bitfield >> 1); }

    Color Piece::color() const { return Color(_bitfield & 1); }

    unsigned Piece::index() const { return color() * 6 + (type() - 1); }

    const char Piece::character() const { return PIECE_CHARS[index()]; }

    const std::string Piece::icon() const { return PIECE_ICONS[index()]; }
} // namespace Brainiac