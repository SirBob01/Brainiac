#include "Piece.hpp"

namespace Brainiac {
    Piece::Piece(PieceType type, Color color) : _type(type), _color(color) {}
    Piece::Piece() : _type(PieceType::King), _color(Color::Empty) {}

    PieceType Piece::type() const { return _type; }

    Color Piece::color() const { return _color; }

    unsigned Piece::index() const {
        return static_cast<uint8_t>(color()) * 6 + static_cast<uint8_t>(type());
    }

    const char Piece::character() const { return PIECE_CHARS[index()]; }

    const std::string Piece::icon() const { return PIECE_ICONS[index()]; }

    bool Piece::empty() const { return _color == Color::Empty; }

    bool Piece::operator==(const Piece &other) const {
        return _type == other._type && _color == other._color;
    }
} // namespace Brainiac