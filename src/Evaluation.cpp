#include "Evaluation.hpp"

namespace Brainiac {
    Value compute_material(const Board &board) {
        Value total = 0;
        for (uint8_t i = 0; i < 12; i++) {
            Piece piece = static_cast<Piece>(i);
            Bitboard bitboard = board.bitboard(piece);
            total += count_set_bitboard(bitboard) * PIECE_WEIGHTS[i];
        }
        return total;
    }

    Value compute_placement(const Board &board) {
        Value total = 0;
        for (uint8_t r = 0; r < 8; r++) {
            for (uint8_t c = 0; c < 8; c++) {
                Square sq = static_cast<Square>(r * 8 + c);
                Piece piece = board.get(sq);
                Color color = get_piece_color(piece);

                // Invert the matrix rows
                uint8_t index = color == Color::White ? ((7 - r) * 8 + c) : sq;

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

    Value evaluate(Position &pos) {
        Value sign = (pos.turn() << 1) - 1;

        // Leaf node
        if (pos.is_checkmate()) {
            return -sign * MAX_VALUE;
        }
        if (pos.is_draw()) {
            return 0;
        }

        // Compute mobility
        Value mobility = pos.moves().size();
        pos.skip();
        mobility -= pos.moves().size();
        pos.undo();

        // Non-leaf node (depth capped)
        const Board &board = pos.board();
        Value material = compute_material(board);
        Value placement = compute_placement(board);

        return sign * 2 * (material + 3 * placement) + mobility;
    }
} // namespace Brainiac