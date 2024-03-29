#include "Board.hpp"

namespace Brainiac {
    Piece Board::get(Square sq) const { return _pieces[sq]; }

    void Board::set(Square sq, Piece piece) {
        Bitboard mask = get_square_mask(sq);
        _bitboards[12 + static_cast<uint8_t>(piece.color())] |= mask;
        _bitboards[piece.index()] |= mask;
        _pieces[sq] = piece;
    }

    Piece Board::get(uint8_t rank, uint8_t file) const {
        return get(Square(rank * 8 + file));
    }

    void Board::set(uint8_t rank, uint8_t file, Piece piece) {
        return set(Square(rank * 8 + file), piece);
    }

    void Board::print() const {
        std::string files = "ABCDEFGH";
        for (int rank = 7; rank >= 0; rank--) {
            std::cout << rank + 1 << " ";
            for (int file = 0; file < 8; file++) {
                Piece piece = get(rank, file);
                if (piece.type() != PieceType::Empty) {
                    std::cout << piece.icon() << " ";
                } else {
                    if (rank % 2 == file % 2) {
                        std::cout << "- ";
                    } else {
                        std::cout << ". ";
                    }
                }
            }
            std::cout << "\n";
        }
        std::cout << "  ";
        for (auto &f : files) {
            std::cout << f << " ";
        }
        std::cout << "\n";
    }
} // namespace Brainiac