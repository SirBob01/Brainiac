#include "Piece.hpp"

namespace Brainiac {
    Piece create_piece(PieceType type, Color color) {
        return static_cast<Piece>(color * 6 + type);
    }

    Color get_piece_color(Piece piece) {
        return static_cast<Color>(piece >= 6);
    }
} // namespace Brainiac