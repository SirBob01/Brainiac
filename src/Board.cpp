#include "Board.hpp"

namespace Brainiac {
    Board::Board() {
        std::fill(_bitboards.begin(), _bitboards.end(), 0);
        std::fill(_pieces.begin(), _pieces.end(), Piece::Empty);
    }

    Bitboard Board::bitboard(Color color) const {
        return _bitboards[13 + color];
    }

    Bitboard Board::bitboard(Piece piece) const { return _bitboards[piece]; }

    Piece Board::get(Square sq) const { return _pieces[sq]; }

    void Board::set(Square sq, Piece piece) {
        Piece prev_piece = get(sq);
        Bitboard set_mask = get_square_mask(sq);
        Bitboard clear_mask = ~set_mask;

        // Clear previous piece bitboards
        _bitboards[13 + (prev_piece >= 6)] &= clear_mask;
        _bitboards[prev_piece] &= clear_mask;

        // Set new piece bitboards
        _bitboards[13 + (piece >= 6)] |= set_mask;
        _bitboards[piece] |= set_mask;

        // Update mailbox
        _pieces[sq] = piece;
    }

    void Board::clear(Square sq) {
        Piece piece = get(sq);
        Bitboard mask = ~get_square_mask(sq);

        _bitboards[13 + (piece >= 6)] &= mask;
        _bitboards[piece] &= mask;

        _pieces[sq] = Piece::Empty;
    }

    void Board::print() const {
        std::string files = "ABCDEFGH";
        for (int rank = 7; rank >= 0; rank--) {
            std::cout << rank + 1 << " ";
            for (int file = 0; file < 8; file++) {
                Piece piece = get(Square(rank * 8 + file));
                if (piece != Piece::Empty) {
                    std::cout << PIECE_ICONS[piece] << " ";
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