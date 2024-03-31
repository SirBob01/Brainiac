#include "Board.hpp"

namespace Brainiac {
    Bitboard Board::bitboard(Color color) const {
        return _bitboards[18 + static_cast<uint8_t>(color)];
    }

    Bitboard Board::bitboard(Piece piece) const {
        return _bitboards[static_cast<uint8_t>(piece.color()) * 6 +
                          static_cast<uint8_t>(piece.type())];
    }

    Piece Board::get(Square sq) const { return _pieces[sq]; }

    void Board::set(Square sq, Piece piece) {
        Piece prev_piece = get(sq);
        Bitboard set_mask = get_square_mask(sq);
        Bitboard clear_mask = ~set_mask;

        // Clear previous piece bitboards
        _bitboards[18 + static_cast<uint8_t>(prev_piece.color())] &= clear_mask;
        _bitboards[prev_piece.index()] &= clear_mask;

        // Set new piece bitboards
        _bitboards[18 + static_cast<uint8_t>(piece.color())] |= set_mask;
        _bitboards[piece.index()] |= set_mask;

        // Update mailbox
        _pieces[sq] = piece;
    }

    void Board::print() const {
        std::string files = "ABCDEFGH";
        for (int rank = 7; rank >= 0; rank--) {
            std::cout << rank + 1 << " ";
            for (int file = 0; file < 8; file++) {
                Piece piece = get(Square(rank * 8 + file));
                if (!piece.empty()) {
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