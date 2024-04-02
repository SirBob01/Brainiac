#include "Piece.hpp"

namespace Brainiac {
    Piece create_piece(PieceType type, Color color) {
        return static_cast<Piece>(color * 6 + type);
    }
} // namespace Brainiac