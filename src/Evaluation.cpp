#include "Evaluation.hpp"
#include "Piece.hpp"

namespace Brainiac {
    short compute_material(const Board &board) {
        short total = 0;
        for (uint8_t i = 0; i < 12; i++) {
            Piece piece = static_cast<Piece>(i);
            Bitboard bitboard = board.bitboard(piece);
            total += count_set_bitboard(bitboard) * PIECE_WEIGHTS[i];
        }
        return total;
    }

    short compute_placement(const Board &board) {
        short total = 0;
        for (uint8_t r = 0; r < 8; r++) {
            for (uint8_t c = 0; c < 8; c++) {
                Square sq = static_cast<Square>(r * 8 + c);
                Piece piece = board.get(sq);
                Color color = get_piece_color(piece);

                // Invert the board if the piece is black
                uint8_t index = color == Color::White ? sq : ((7 - r) * 8 + c);

                switch (piece) {
                // White pieces
                case Piece::WhitePawn:
                    total += PAWN_MATRIX[index];
                    break;
                case Piece::WhiteKnight:
                    total += KNIGHT_MATRIX[index];
                    break;
                case Piece::WhiteBishop:
                    total += BISHOP_MATRIX[index];
                    break;
                case Piece::WhiteRook:
                    total += ROOK_MATRIX[index];
                    break;
                case Piece::WhiteQueen:
                    total += QUEEN_MATRIX[index];
                    break;
                case Piece::WhiteKing:
                    total += KING_MATRIX[index];
                    break;

                // Black pieces
                case Piece::BlackPawn:
                    total -= PAWN_MATRIX[index];
                    break;
                case Piece::BlackKnight:
                    total -= KNIGHT_MATRIX[index];
                    break;
                case Piece::BlackBishop:
                    total -= BISHOP_MATRIX[index];
                    break;
                case Piece::BlackRook:
                    total -= ROOK_MATRIX[index];
                    break;
                case Piece::BlackQueen:
                    total -= QUEEN_MATRIX[index];
                    break;
                case Piece::BlackKing:
                    total -= KING_MATRIX[index];
                    break;

                default:
                    break;
                }
            }
        }
        return total;
    }

    short evaluate(Position &pos) {
        short sign = (pos.turn() << 1) - 1;

        // Leaf node
        if (pos.is_checkmate()) {
            return -sign * UPPER_BOUND;
        }
        if (pos.is_draw()) {
            return 0;
        }

        // Non-leaf node (depth capped)
        const Board &board = pos.board();
        short material = compute_material(board);
        short placement = compute_placement(board);
        return sign * (material + 2 * placement);
    }
} // namespace Brainiac