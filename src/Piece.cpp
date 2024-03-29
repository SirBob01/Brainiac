#include "Piece.hpp"

namespace Brainiac {
    Piece::Piece(PieceType type, Color color) :
        _bitfield(static_cast<uint8_t>(type) << 1 |
                  static_cast<uint8_t>(color)){};

    Piece::Piece() : Piece(PieceType::Empty, Color::White){};

    PieceType Piece::type() const { return PieceType(_bitfield >> 1); }

    Color Piece::color() const { return Color(_bitfield & 1); }

    unsigned Piece::index() const {
        return static_cast<uint8_t>(color()) * 6 + static_cast<uint8_t>(type());
    }

    const char Piece::character() const { return PIECE_CHARS[index()]; }

    const std::string Piece::icon() const { return PIECE_ICONS[index()]; }
} // namespace Brainiac