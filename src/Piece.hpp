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
     * @brief Piece types.
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
     * @brief Piece colors.
     *
     */
    enum Color : uint8_t {
        White,
        Black,
    };
} // namespace Brainiac