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
    enum class PieceType : uint8_t {
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
    enum class Color : uint8_t {
        White,
        Black,
        Empty,
    };

    /**
     * @brief Piece representation.
     *
     */
    class Piece {
        PieceType _type;
        Color _color;

      public:
        /**
         * @brief Construct a new Piece.
         *
         * @param type
         * @param color
         */
        Piece(PieceType type, Color color);
        Piece();

        /**
         * @brief Get the piece type.
         *
         * @return PieceType
         */
        PieceType type() const;

        /**
         * @brief Get the piece color.
         *
         * @return Color
         */
        Color color() const;

        /**
         * @brief Get the piece index.
         *
         * @return unsigned
         */
        unsigned index() const;

        /**
         * @brief Get the character representation.
         *
         * @return const char
         */
        const char character() const;

        /**
         * @brief Get the Unicode icon representation.
         *
         * @return const std::string
         */
        const std::string icon() const;

        /**
         * @brief Test if the piece is empty.
         *
         * @return true
         * @return false
         */
        bool empty() const;

        /**
         * @brief Test equality with another piece.
         *
         * @param other
         * @return true
         * @return false
         */
        bool operator==(const Piece &other) const;
    };
} // namespace Brainiac