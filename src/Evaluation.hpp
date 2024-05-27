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
        100,
        10,
        50,
        32,
        33,
        90,
        -100,
        -10,
        -50,
        -32,
        -33,
        -90,
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
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
        -5,  0,  5,  5,  5,  5,  0, -5,
        0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    };
    constexpr std::array<Value, 64> BISHOP_MATRIX = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20,
    };
    constexpr std::array<Value, 64> KNIGHT_MATRIX = {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50,
    };
    constexpr std::array<Value, 64> ROOK_MATRIX = {
        0,  0,  0,  0,  0,  0,  0,  0,
        5, 10, 10, 10, 10, 10, 10,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        0,  0,  0,  5,  5,  0,  0,  0
    };
    constexpr std::array<Value, 64> PAWN_MATRIX = {
        0,  0,  0,  0,  0,  0,  0,  0,
        70, 70, 70, 70, 70, 70, 70, 70,
        10, 10, 20, 40, 40, 20, 10, 10,
        5,  5, 10, 35, 35, 10,  5,  5,
        0,  0,  0, 30, 30,  0,  0,  0,
        5, -5,-10,  0,  0,-10, -5,  5,
        5, 10, 10,-20,-20, 10, 10,  5,
        0,  0,  0,  0,  0,  0,  0,  0,
    };
    constexpr std::array<Value, 64> KING_MATRIX = {
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
        20, 20,  0,  0,  0,  0, 20, 20,
        20, 30, 10,  0,  0, 10, 30, 20
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