#pragma once

#include <array>

#include "Numeric.hpp"
#include "Position.hpp"

namespace Brainiac {
    /**
     * @brief Piece weights from white's perspective.
     *
     */
    constexpr std::array<Value, 12> PIECE_WEIGHTS = {
        0,
        1,
        5,
        3,
        3,
        9,
        0,
        -1,
        -5,
        -3,
        -3,
        -9,
    };

    /**
     * @brief The matrices below determine the placement scores of each piece
     * on the board.
     *
     * The higher the score at a particular index, the more favorable
     * that location for that piece.
     *
     * Note that value placement is from the perspective of white from the
     * visual representation of the board, i.e., starting position is on the
     * bottom 2 rows.
     */
    // clang-format off
    constexpr std::array<Value, 64> QUEEN_MATRIX = {
        -4, -2, -2, -1, -1, -2, -2, -4, 
        -2, 0, 0, 0, 0, 0, 0, -2,
        -2, 0, 1, 1, 1, 1, 0, -2, 
        -1, 0, 1, 1, 1, 1, 0, -1,
        0, 0, 1, 1, 1, 1, 0, -1, 
        -2, 1, 1, 1, 1, 1, 0, -2,
        -2, 0, 1, 0, 0, 0, 0, -2, 
        -4, -2, -2, -1, -1, -2, -2, -4,
    };
    constexpr std::array<Value, 64> BISHOP_MATRIX = {
        -2, -1, -1, -1, -1, -1, -1, -2, 
        -1, 0, 0, 0, 0, 0, 0, -1,
        -1, 0, 5, 1, 1, 5, 0, -1, 
        -1, 5, 5, 1, 1, 5, 5, -1,
        -1, 0, 1, 1, 1, 1, 0, -1, 
        -1, 1, 1, 1, 1, 1, 1, -1,
        -1, 5, 0, 0, 0, 0, 5, -1, 
        -2, -1, -5, -1, -1, -5, -1, -2,
    };
    constexpr std::array<Value, 64> KNIGHT_MATRIX = {
        -5, -4, -3, -3, -3, -3, -4, -5, 
        -4, -2, 0, 0, 0, 0, -2, -4,
        -3, 0, 1, 2, 2, 1, 0, -3, 
        -3, 5, 2, 3, 3, 2, 5, -3,
        -3, 0, 2, 3, 3, 2, 0, -3, 
        -3, 5, 1, 2, 2, 1, 5, -3,
        -4, -2, 0, 5, 5, 0, -2, -4, 
        -5, -5, -2, -3, -3, -2, -5, -5,
    };
    constexpr std::array<Value, 64> ROOK_MATRIX = {
        0, 0, 0, 0, 0, 0, 0, 0,  
        1, 2, 2, 2, 2, 2, 2, 1,
        -1, 0, 0, 0, 0, 0, 0, -1, 
        -1, 0, 0, 0, 0, 0, 0, -1,
        -1, 0, 0, 0, 0, 0, 0, -1, 
        -1, 0, 0, 0, 0, 0, 0, -1,
        -1, 0, 0, 0, 0, 0, 0, -1, 
        -3, 3, 4, 1, 1, 0, 0, -3,
    };
    constexpr std::array<Value, 64> PAWN_MATRIX = {
        0, 0, 0, 0, 0, 0, 0, 0, 
        7, 7, 7, 7, 7, 7, 7, 7,
        1, 1, 2, 3, 3, 2, 1, 1, 
        5, 5, 1, 8, 8, 1, 5, 5,
        0, 0, 0, 3, 3, 0, 0, 0, 
        5, -5, -1, 0, 0, -1, -5, 5,
        5, 1, 1, -3, -3, 1, 1, 5, 
        0, 0, 0, 0, 0, 0, 0, 0,
    };
    constexpr std::array<Value, 64> KING_MATRIX = {
        -3, -4, -4, -5, -5, -4, -4, -3, 
        -3, -4, -4, -5, -5, -4, -4, -3,
        -3, -4, -4, -5, -5, -4, -4, -3, 
        -3, -4, -4, -5, -5, -4, -4, -3,
        -2, -3, -3, -4, -4, -3, -3, -2, 
        -1, -2, -2, -2, -2, -2, -2, -1,
        2, 2, 0, 0, 0, 0, 2, 2,  
        2, 3, 1, 0, 0, 1, 3, 2,
    };
    // clang-format on

    /**
     * @brief Compute the material score from white's perspective.
     *
     * @param board
     * @return Value
     */
    Value compute_material(const Board &board);

    /**
     * @brief Compute the placement score from white's perspective.
     *
     * @param board
     * @return Value
     */
    Value compute_placement(const Board &board);

    /**
     * @brief Evaluate a position for the current turn.
     *
     * @param pos
     * @return Value
     */
    Value evaluate(Position &pos);
} // namespace Brainiac