#pragma once

#include <array>

#include "Position.hpp"
#include "Transpositions.hpp"

namespace Brainiac {
    /**
     * @brief Piece weights from white's perspective.
     *
     */
    constexpr std::array<short, 12> PIECE_WEIGHTS = {
        40,
        10,
        55,
        30,
        33,
        95,
        -40,
        -10,
        -55,
        -30,
        -33,
        -95,
    };

    /**
     * @brief The matrices below determine the placement scores of each piece
     * on the board.
     *
     * The higher the score at a particular index, the more favorable
     * that location for that piece.
     *
     * Note that value placement matches the visual representation of the board,
     * i.e., starting position is on the bottom 2 rows.
     */
    // clang-format off
    const short QUEEN_MATRIX[64] = {
        -40, -20, -20, -10, -10, -20, -20, -40, 
        -20, 00, 00, 00, 00, 00, 00, -20,
        -20, 00, 10, 10, 10, 10, 00, -20, 
        -10, 00, 10, 10, 10, 10, 00, -10,
        00, 00, 10, 10, 10, 10, 00, -10, 
        -20, 10, 10, 10, 10, 10, 00, -20,
        -20, 00, 10, 00, 00, 00, 00, -20, 
        -40, -20, -20, -10, -10, -20, -20, -40,
    };
    const short BISHOP_MATRIX[64] = {
        -20, -10, -10, -10, -10, -10, -10, -20, 
        -10, 00, 00, 00, 00, 00, 00, -10,
        -10, 00, 50, 10, 10, 50, 00, -10, 
        -10, 50, 50, 10, 10, 50, 50, -10,
        -10, 00, 10, 10, 10, 10, 00, -10, 
        -10, 10, 10, 10, 10, 10, 10, -10,
        -10, 50, 00, 00, 00, 00, 50, -10, 
        -20, -10, -50, -10, -10, -50, -10, -20,
    };
    const short KNIGHT_MATRIX[64] = {
        -50, -40, -30, -30, -30, -30, -40, -50, 
        -40, -20, 00, 00, 00, 00, -20, -40,
        -30, 00, 10, 15, 15, 10, 00, -30, 
        -30, 50, 15, 20, 20, 15, 50, -30,
        -30, 00, 15, 20, 20, 15, 00, -30, 
        -30, 50, 10, 15, 15, 10, 50, -30,
        -40, -20, 00, 50, 50, 00, -20, -40, 
        -50, -50, -20, -30, -30, -20, -50, -50,
    };
    const short ROOK_MATRIX[64] = {
        00, 00, 00, 00, 00, 00, 00, 00,  
        10, 20, 20, 20, 20, 20, 20, 10,
        -10, 00, 00, 00, 00, 00, 00, -10, 
        -10, 00, 00, 00, 00, 00, 00, -10,
        -10, 00, 00, 00, 00, 00, 00, -10, 
        -10, 00, 00, 00, 00, 00, 00, -10,
        -10, 00, 00, 00, 00, 00, 00, -10, 
        -30, 30, 40, 10, 10, 00, 00, -30,
    };
    const short PAWN_MATRIX[64] = {
        00, 00, 00, 00, 00, 00, 00, 00, 
        70, 70, 70, 70, 70, 70, 70, 70,
        10, 10, 20, 30, 30, 20, 10, 10, 
        50, 50, 10, 75, 75, 10, 50, 50,
        00, 00, 00, 25, 25, 00, 00, 00, 
        50, -50, -10, 00, 00, -10, -50, 50,
        50, 10, 10, -25, -25, 10, 10, 50, 
        00, 00, 00, 00, 00, 00, 00, 00,
    };
    const short KING_MATRIX[64] = {
        -30, -40, -40, -50, -50, -40, -40, -30, 
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30, 
        -30, -40, -40, -50, -50, -40, -40, -30,
        -20, -30, -30, -40, -40, -30, -30, -20, 
        -10, -20, -20, -20, -20, -20, -20, -10,
        20, 20, 00, 00, 00, 00, 20, 20,  
        20, 30, 10, 00, 00, 10, 30, 20,
    };
    // clang-format on

    /**
     * @brief Compute the material score from white's perspective.
     *
     * @param board
     * @return short
     */
    short compute_material(const Board &board);

    /**
     * @brief Compute the placement score from white's perspective.
     *
     * @param board
     * @return short
     */
    short compute_placement(const Board &board);

    /**
     * @brief Evaluate a position for the current turn.
     *
     * @param pos
     * @return short
     */
    short evaluate(Position &pos);
} // namespace Brainiac