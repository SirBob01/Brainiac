#ifndef HEURISTIC_H_
#define HEURISTIC_H_

#include "board.h"
#include "piece.h"

namespace chess {
    /**
     * The matrices below determine the placement scores of each piece
     * on the board
     *
     * The higher the score at a particular index, the more favorable
     * that location for that piece
     */
    const float queen_matrix[64] = {
        -4, -2, -2, -1, -1, -2, -2, -4, -2, 0,  0,  0,  0,  0,  0,  -2,
        -2, 0,  1,  1,  1,  1,  0,  -2, -1, 0,  1,  1,  1,  1,  0,  -1,
        0,  0,  1,  1,  1,  1,  0,  -1, -2, 1,  1,  1,  1,  1,  0,  -2,
        -2, 0,  1,  0,  0,  0,  0,  -2, -4, -2, -2, -1, -1, -2, -2, -4};
    const float bishop_matrix[64] = {
        -2, -1, -1, -1, -1, -1, -1, -2, -1, 0,  0,  0,  0,  0,  0,  -1,
        -1, 0,  5,  1,  1,  5,  0,  -1, -1, 5,  5,  1,  1,  5,  5,  -1,
        -1, 0,  1,  1,  1,  1,  0,  -1, -1, 1,  1,  1,  1,  1,  1,  -1,
        -1, 5,  0,  0,  0,  0,  5,  -1, -2, -1, -4, -1, -1, -4, -1, -2,
    };
    const float knight_matrix[64] = {
        -5, -4, -3,  -3,  -3,  -3,  -4, -5, -4, -2, 0,   0,   0,   0,   -2, -4,
        -3, 0,  1,   1.5, 1.5, 1,   0,  -3, -3, 5,  1.5, 2,   2,   1.5, 5,  -3,
        -3, 0,  1.5, 2,   2,   1.5, 0,  -3, -3, 5,  1,   1.5, 1.5, 1,   5,  -3,
        -4, -2, 0,   5,   5,   0,   -2, -4, -5, -4, -2,  -3,  -3,  -2,  -4, -5,
    };
    const float rook_matrix[64] = {
        0,  0, 0, 0, 0, 0, 0, 0,  1,  2, 2, 2, 2, 2, 2, 1,
        -1, 0, 0, 0, 0, 0, 0, -1, -1, 0, 0, 0, 0, 0, 0, -1,
        -1, 0, 0, 0, 0, 0, 0, -1, -1, 0, 0, 0, 0, 0, 0, -1,
        -1, 0, 0, 0, 0, 0, 0, -1, -3, 3, 4, 1, 1, 0, 0, -3};
    const float pawn_matrix[64] = {
        0, 0, 0, 0,    0,    0, 0, 0, 5, 5,  5,  5,  5,  5,  5,  5,
        1, 1, 2, 3,    3,    2, 1, 1, 5, 5,  1,  27, 27, 1,  5,  5,
        0, 0, 0, 2.5,  2.5,  0, 0, 0, 5, -5, -1, 0,  0,  -1, -5, 5,
        5, 1, 1, -2.5, -2.5, 1, 1, 5, 0, 0,  0,  0,  0,  0,  0,  0};
    const float king_matrix[64] = {
        -3, -4, -4, -5, -5, -4, -4, -3, -3, -4, -4, -5, -5, -4, -4, -3,
        -3, -4, -4, -5, -5, -4, -4, -3, -3, -4, -4, -5, -5, -4, -4, -3,
        -2, -3, -3, -4, -4, -3, -3, -2, -1, -2, -2, -2, -2, -2, -2, -1,
        2,  2,  0,  0,  0,  0,  2,  2,  2,  3,  1,  0,  0,  1,  3,  2};

    /**
     * Calculate the material score of the board
     */
    inline float material_score(Board &board, Color maximizer) {
        float material = board.calculate_material();
        if (maximizer == Color::Black) {
            return -material;
        }
        return material;
    }

    /**
     * Calculate the placement score of the board
     */
    inline float placement_score(Board &board, Color maximizer) {
        float score = 0;
        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 8; c++) {
                Piece piece = board.get_at_coords(r, c);
                if (piece.is_empty()) {
                    continue;
                }
                int shift = r * 8 + c;
                int sign = piece.color == Color::Black ? -1 : 1;

                switch (piece.type) {
                case PieceType::Queen:
                    score += queen_matrix[shift] * sign;
                    break;
                case PieceType::Bishop:
                    score += bishop_matrix[shift] * sign;
                    break;
                case PieceType::Knight:
                    score += knight_matrix[shift] * sign;
                    break;
                case PieceType::Rook:
                    score += rook_matrix[shift] * sign;
                    break;
                case PieceType::Pawn:
                    score += pawn_matrix[shift] * sign;
                    break;
                case PieceType::King:
                    score += king_matrix[shift] * sign;
                    break;
                default:
                    break;
                }
            }
        }
        if (maximizer == Color::Black) {
            return -score;
        }
        return score;
    }

    /**
     * Calculate the MMV-LVA heuristic of the board
     */
    inline float mmv_lva_heuristic(Board &board, const Move &move) {
        Piece attacker = board.get_at(move.from);
        Piece victim = board.get_at(move.to);
        if (victim.is_empty()) {
            return 0;
        }
        float mvv = piece_weights[victim.type];
        float lva = piece_weights[attacker.type];
        return mvv - lva;
    }
} // namespace chess

#endif