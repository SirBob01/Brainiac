#pragma once

#include <cstdint>
#include <string>

namespace Brainiac {
    /**
     * @brief ASCII characters for each piece (white - black)
     *
     */
    static const std::string PIECE_CHARS = "KPRNBQkprnbq";

    /**
     * @brief Unicode to visualize a piece on the terminal (white - black)
     *
     */
    static const std::string PIECE_ICONS[] = {
        "\u2654",
        "\u2659",
        "\u2656",
        "\u2658",
        "\u2657",
        "\u2655",
        "\u265A",
        "\u265F",
        "\u265C",
        "\u265E",
        "\u265D",
        "\u265B",
    };

    /**
     * @brief All piece types for all colors.
     *
     */
    enum Piece : uint8_t {
        WhiteKing,
        WhitePawn,
        WhiteRook,
        WhiteKnight,
        WhiteBishop,
        WhiteQueen,

        BlackKing,
        BlackPawn,
        BlackRook,
        BlackKnight,
        BlackBishop,
        BlackQueen,

        Empty,
    };

    /**
     * @brief Piece types.
     *
     */
    enum PieceType : uint8_t {
        King,
        Pawn,
        Rook,
        Knight,
        Bishop,
        Queen,
    };

    /**
     * @brief Piece colors.
     *
     */
    enum Color : uint8_t {
        White,
        Black,
    };

    /**
     * @brief Create a Piece given a type and color.
     *
     * @param type
     * @param color
     * @return Piece
     */
    Piece create_piece(PieceType type, Color color);
} // namespace Brainiac