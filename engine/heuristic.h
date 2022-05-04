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
     * Calculate the mobility heuristic of the board state
     *
     * This performs a null move to get the mobility score of the opposing
     * player
     */
    inline float mobility_score(Board &board) {
        float mobility = board.get_mobility();
        board.skip_turn();
        mobility -= board.get_mobility();
        board.undo_move();

        if (board.get_turn() == Color::Black) {
            return -mobility;
        }
        return mobility;
    }

    /**
     * Calculate the placement score of the board
     */
    inline float placement_score(Board &board) {
        float score = 0;
        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 8; c++) {
                Piece piece = board.get_at_coords(r, c);
                if (piece.is_empty()) {
                    continue;
                }
                int shift = r * 8 + c;
                int sign = piece.color == Color::White ? 1 : -1;

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
        return score;
    }

    /**
     * Calculate the MVV-LVA heuristic of the board
     */
    inline float mvv_lva_heuristic(Board &board, const Move &move) {
        Piece attacker = board.get_at(move.from);
        Piece victim = board.get_at(move.to);

        float mvv = piece_weights[victim.type];
        float lva = piece_weights[attacker.type];
        return mvv - lva;
    }

    /**
     * Calculate the static exchange evaluation for a square
     */
    inline int static_exchange_evaluation(Board &board, const Square &square) {
        int value = 0;
        const std::vector<Move> &moves = board.get_moves();

        Piece victim = board.get_at(square);
        int victim_value = std::fabs(piece_weights[victim.get_piece_index()]);

        Move best_move;
        int min_value = 100000; // Some arbitrary large number lol
        for (const Move &move : moves) {
            if (move.flags & MoveFlag::Capture) {
                Piece attacker = board.get_at(move.from);
                int attacker_value =
                    std::fabs(piece_weights[attacker.get_piece_index()]);
                if (attacker_value < min_value) {
                    min_value = attacker_value;
                }
                best_move = move;
            }
        }

        if (!best_move.is_invalid()) {
            board.execute_move(best_move);
            int see = static_exchange_evaluation(board, square);
            value = std::max(0, victim_value - see);
            board.undo_move();
        }
        return value;
    }

    /**
     * Calculate the resulting SEE heuristic after performing a capture move
     */
    inline float see_heuristic(Board &board, const Move &move) {
        Piece victim = board.get_at(move.to);
        float value = piece_weights[victim.get_piece_index()];
        board.execute_move(move);
        value -= static_exchange_evaluation(board, move.to);
        board.undo_move();
        return value;
    }
} // namespace chess

#endif