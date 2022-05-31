#ifndef HEURISTIC_H_
#define HEURISTIC_H_

#include "board.h"
#include "piece.h"

namespace brainiac {
    /**
     * @brief Material weight of each piece type (and color)
     *
     */
    // clang-format off
    constexpr float piece_weights[] ={
         4,  1,  5.5,  3.05,  3.33,  9.5, 
        -4, -1, -5.5, -3.05, -3.33, -9.5
    };

    /**
     * @brief The matrices below determine the placement scores of each piece
     * on the board
     *
     * The higher the score at a particular index, the more favorable
     * that location for that piece
     *
     * Note that value placement matches the visual representation of the board,
     * i.e., starting position is on the bottom 2 rows
     */
    const float queen_matrix[64] = {
        -4, -2, -2, -1, -1, -2, -2, -4, 
        -2, 0,  0,  0,  0,  0,  0,  -2,
        -2, 0,  1,  1,  1,  1,  0,  -2, 
        -1, 0,  1,  1,  1,  1,  0,  -1,
        0,  0,  1,  1,  1,  1,  0,  -1, 
        -2, 1,  1,  1,  1,  1,  0,  -2,
        -2, 0,  1,  0,  0,  0,  0,  -2, 
        -4, -2, -2, -1, -1, -2, -2, -4
    };
    const float bishop_matrix[64] = {
        -2, -1, -1, -1, -1, -1, -1, -2, 
        -1, 0,  0,  0,  0,  0,  0,  -1,
        -1, 0,  5,  1,  1,  5,  0,  -1, 
        -1, 5,  5,  1,  1,  5,  5,  -1,
        -1, 0,  1,  1,  1,  1,  0,  -1, 
        -1, 1,  1,  1,  1,  1,  1,  -1,
        -1, 5,  0,  0,  0,  0,  5,  -1, 
        -2, -1, -4, -1, -1, -4, -1, -2,
    };
    const float knight_matrix[64] = {
        -5, -4, -3,  -3,  -3,  -3,  -4, -5, 
        -4, -2, 0,   0,   0,   0,   -2, -4,
        -3, 0,  1,   1.5, 1.5, 1,   0,  -3, 
        -3, 5,  1.5, 2,   2,   1.5, 5,  -3,
        -3, 0,  1.5, 2,   2,   1.5, 0,  -3, 
        -3, 5,  1,   1.5, 1.5, 1,   5,  -3,
        -4, -2, 0,   5,   5,   0,   -2, -4, 
        -5, -4, -2,  -3,  -3,  -2,  -4, -5,
    };
    const float rook_matrix[64] = {
        0,  0, 0, 0, 0, 0, 0, 0,  
        1,  2, 2, 2, 2, 2, 2, 1,
        -1, 0, 0, 0, 0, 0, 0, -1, 
        -1, 0, 0, 0, 0, 0, 0, -1,
        -1, 0, 0, 0, 0, 0, 0, -1, 
        -1, 0, 0, 0, 0, 0, 0, -1,
        -1, 0, 0, 0, 0, 0, 0, -1, 
        -3, 3, 4, 1, 1, 0, 0, -3
    };
    const float pawn_matrix[64] = {
        0, 0, 0, 0,    0,    0, 0, 0, 
        5, 5,  5,  5,  5,  5,  5,  5,
        1, 1, 2, 3,    3,    2, 1, 1, 
        5, 5,  1,  27, 27, 1,  5,  5,
        0, 0, 0, 2.5,  2.5,  0, 0, 0, 
        5, -5, -1, 0,  0,  -1, -5, 5,
        5, 1, 1, -2.5, -2.5, 1, 1, 5, 
        0, 0,  0,  0,  0,  0,  0,  0
    };
    const float king_matrix[64] = {
        -3, -4, -4, -5, -5, -4, -4, -3, 
        -3, -4, -4, -5, -5, -4, -4, -3,
        -3, -4, -4, -5, -5, -4, -4, -3, 
        -3, -4, -4, -5, -5, -4, -4, -3,
        -2, -3, -3, -4, -4, -3, -3, -2, 
        -1, -2, -2, -2, -2, -2, -2, -1,
        2,  2,  0,  0,  0,  0,  2,  2,  
        2,  3,  1,  0,  0,  1,  3,  2
    };
    // clang-format on

    /**
     * @brief Calculate the material score of the current board state
     *
     * @param board
     * @return float
     */
    inline float material_score(Board &board) {
        float score = 0;
        for (int i = 0; i < 12; i++) {
            score += count_set_bits(board.get_bitboard(i)) * piece_weights[i];
        }
        return score;
    }

    /**
     * @brief Value of a move for mobility score calculation
     *
     * @param move
     * @return float
     */
    inline float move_mobility_heuristic(Move move) {
        float score = 1;
        MoveFlagSet flags = move.get_flags();
        if (flags & MoveFlag::Castling) {
            score += 2;
        } else {
            if (flags & (MoveFlag::BishopPromo | MoveFlag::QueenPromo |
                         MoveFlag::RookPromo | MoveFlag::KnightPromo)) {
                score += 5;
            }
            if (flags & MoveFlag::Capture) {
                score += 3;
            }
        }
        return score;
    }

    /**
     * @brief Calculate the mobility heuristic of the board state
     *
     * This performs a null move to get the mobility score of the opposing
     * player
     *
     * @param board
     * @return float
     */
    inline float mobility_score(Board &board) {
        float mobility = 0;

        // Calculate mobility of current turn
        MoveList &curr_moves = board.get_moves();
        for (int i = 0; i < curr_moves.size(); i++) {
            mobility += move_mobility_heuristic(curr_moves[i]);
        }

        // Calculate mobility of opposing player
        board.skip_move();
        MoveList &next_moves = board.get_moves();
        for (int i = 0; i < next_moves.size(); i++) {
            mobility -= move_mobility_heuristic(next_moves[i]);
        }
        board.undo_move();

        if (board.get_turn() == Color::Black) {
            return -mobility;
        }
        return mobility;
    }

    /**
     * @brief Utility function for calculating the total placement score for a
     * piece's bitboard
     *
     * @param bitboard
     * @param matrix
     * @return float
     */
    inline float piece_placement_total(Bitboard bitboard,
                                       const float matrix[64]) {
        float score = 0;
        while (bitboard) {
            Bitboard unit = get_lsb(bitboard);
            score += matrix[find_lsb(unit)];
            bitboard = pop_lsb(bitboard);
        }
        return score;
    }

    /**
     * @brief Calculate the placement score of the board using the piece-square
     * matrices
     *
     * @param board
     * @return float
     */
    inline float placement_score(Board &board) {
        // White pieces
        Bitboard w_p =
            flip_vertical(board.get_bitboard(PieceType::Pawn, Color::White));
        Bitboard w_n =
            flip_vertical(board.get_bitboard(PieceType::Knight, Color::White));
        Bitboard w_b =
            flip_vertical(board.get_bitboard(PieceType::Bishop, Color::White));
        Bitboard w_r =
            flip_vertical(board.get_bitboard(PieceType::Rook, Color::White));
        Bitboard w_q =
            flip_vertical(board.get_bitboard(PieceType::Queen, Color::White));
        Bitboard w_k =
            flip_vertical(board.get_bitboard(PieceType::King, Color::White));

        // Black pieces
        Bitboard b_p = board.get_bitboard(PieceType::Pawn, Color::Black);
        Bitboard b_n = board.get_bitboard(PieceType::Knight, Color::Black);
        Bitboard b_b = board.get_bitboard(PieceType::Bishop, Color::Black);
        Bitboard b_r = board.get_bitboard(PieceType::Rook, Color::Black);
        Bitboard b_q = board.get_bitboard(PieceType::Queen, Color::Black);
        Bitboard b_k = board.get_bitboard(PieceType::King, Color::Black);

        // Sum the total placement heuristic
        return (piece_placement_total(w_p, pawn_matrix) +
                piece_placement_total(w_n, knight_matrix) +
                piece_placement_total(w_b, bishop_matrix) +
                piece_placement_total(w_r, rook_matrix) +
                piece_placement_total(w_q, queen_matrix) +
                piece_placement_total(w_k, king_matrix)) -
               (piece_placement_total(b_p, pawn_matrix) +
                piece_placement_total(b_n, knight_matrix) +
                piece_placement_total(b_b, bishop_matrix) +
                piece_placement_total(b_r, rook_matrix) +
                piece_placement_total(b_q, queen_matrix) +
                piece_placement_total(b_k, king_matrix));
    }

    /**
     * @brief Calculate the heuristic based on the number of connected pawns for
     * each side (from white's perspective)
     *
     * @param board
     * @return float
     */
    inline float connected_pawn_score(Board &board) {
        Bitboard white_pawns =
            board.get_bitboard(PieceType::Pawn, Color::White);
        Bitboard black_pawns =
            board.get_bitboard(PieceType::Pawn, Color::Black);

        float connected = 0;
        for (int i = 0; i < 8; i++) {
            if (i < 7) {
                if ((files[i] & white_pawns) && (files[i + 1] & white_pawns)) {
                    connected++;
                }
                if ((files[i] & black_pawns) && (files[i + 1] & black_pawns)) {
                    connected--;
                }
            }
            if (i > 0) {
                if ((files[i] & white_pawns) && (files[i - 1] & white_pawns)) {
                    connected++;
                }
                if ((files[i] & black_pawns) && (files[i - 1] & black_pawns)) {
                    connected--;
                }
            }
        }
        return connected;
    }

    /**
     * @brief Calculate the heuristic based on the number of passed pawns from
     * each side (from white's perspective)
     *
     * Passed pawns have no opposing pawns ahead of it on its left, current, and
     * right files
     *
     * @param board
     * @return float
     */
    inline float passed_pawn_score(Board &board) {
        Bitboard temp;
        Bitboard white_pawns =
            board.get_bitboard(PieceType::Pawn, Color::White);
        Bitboard black_pawns =
            board.get_bitboard(PieceType::Pawn, Color::Black);

        float passed = 0;

        // Calculate passed pawns for white
        temp = white_pawns;
        while (temp) {
            int shift = find_lsb(temp);
            int rank = shift / 8;
            int file = shift % 8;

            Bitboard mask = 0;
            Bitboard file_mask = files[file];
            if (file > 0) file_mask |= files[file - 1];
            if (file < 7) file_mask |= files[file + 1];
            for (int r = rank + 1; r < 8; r++) {
                mask |= (ranks[r] & file_mask);
            }
            if (count_set_bits(mask & black_pawns) == 0) {
                passed++;
            }
            temp = pop_lsb(temp);
        }

        // Calculate passed pawns for black
        temp = black_pawns;
        while (temp) {
            int shift = find_lsb(temp);
            int rank = shift / 8;
            int file = shift % 8;

            Bitboard mask = 0;
            Bitboard file_mask = files[file];
            if (file > 0) file_mask |= files[file - 1];
            if (file < 7) file_mask |= files[file + 1];
            for (int r = rank - 1; r >= 0; r--) {
                mask |= (ranks[r] & file_mask);
            }
            if (count_set_bits(mask & white_pawns) == 0) {
                passed--;
            }
            temp = pop_lsb(temp);
        }
        return passed;
    }

    /**
     * @brief The side with both bishops has a tactical advantage over the side
     * with only one. This is because a bishop only covers EITHER a white or
     * black square ONLY.
     *
     * Losing a bishop means losing control of half the board
     *
     * @param board
     * @return float
     */
    inline float bishop_pair_score(Board &board) {
        Bitboard white_bishops =
            board.get_bitboard(PieceType::Bishop, Color::White);
        Bitboard black_bishops =
            board.get_bitboard(PieceType::Bishop, Color::Black);

        float score = 0;
        if (count_set_bits(white_bishops) == 2) {
            score++;
        }
        if (count_set_bits(black_bishops) == 2) {
            score--;
        }
        return score;
    }

    /**
     * @brief Calculate the MVV-LVA heuristic of a move on the board
     *
     * @param board
     * @param move
     * @return float
     */
    inline float mvv_lva_heuristic(Board &board, const Move &move) {
        Piece attacker = board.get_at(move.get_from());
        Piece victim = board.get_at(move.get_to());

        float mvv = piece_weights[victim.type];
        float lva = piece_weights[attacker.type];
        return mvv - lva;
    }

    /**
     * @brief Calculate the static exchange evaluation for a square
     *
     * @param board
     * @param square
     * @return int
     */
    inline int static_exchange_evaluation(Board &board, const Square square) {
        int value = 0;
        MoveList &moves = board.get_moves();

        Piece victim = board.get_at(square);
        int victim_value = std::fabs(piece_weights[victim.get_index()]);

        Move best_move;
        float min_value = INFINITY;
        for (Move &move : moves) {
            if (move.get_flags() & MoveFlag::Capture) {
                Piece attacker = board.get_at(move.get_from());
                float attacker_value =
                    std::fabs(piece_weights[attacker.get_index()]);
                if (attacker_value < min_value) {
                    min_value = attacker_value;
                }
                best_move = move;
            }
        }

        if (!best_move.is_invalid()) {
            board.make_move(best_move);
            int see = static_exchange_evaluation(board, square);
            value = std::max(0, victim_value - see);
            board.undo_move();
        }
        return value;
    }

    /**
     * @brief Calculate the resulting SEE heuristic after making a capture move
     *
     * @param board
     * @param move
     * @return float
     */
    inline float see_heuristic(Board &board, const Move &move) {
        Piece victim = board.get_at(move.get_to());
        float value = piece_weights[victim.get_index()];
        board.make_move(move);
        value -= static_exchange_evaluation(board, move.get_to());
        board.undo_move();
        return value;
    }
} // namespace brainiac

#endif