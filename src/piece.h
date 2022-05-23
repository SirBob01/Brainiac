#ifndef BRAINIAC_PIECE_H_
#define BRAINIAC_PIECE_H_

#include <cassert>

namespace brainiac {
    /**
     * @brief ASCII characters for each piece (white - black)
     *
     */
    static const char *piece_chars = "KPRNBQkprnbq";

    /**
     * @brief Unicode to visualize a piece on the terminal (white - black)
     *
     */
    static const char *piece_icons[] = {"\u2654",
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
                                        "\u265B"};

    /**
     * @brief Material weight of each piece type
     *
     */
    constexpr int piece_weights[] = {4, 1, 5, 3, 3, 9, -4, -1, -5, -3, -3, -9};

    /**
     * @brief Enumerates the types of a piece
     *
     */
    enum PieceType {
        King = 0,
        Pawn = 1,
        Rook = 2,
        Knight = 3,
        Bishop = 4,
        Queen = 5,
        NPieces = 6
    };

    /**
     * @brief Enumerates the colors (and empty)
     *
     */
    enum Color { White = 0, Black = 1, Empty = 2 };

    /**
     * @brief A piece on the board
     *
     */
    struct Piece {
        PieceType type = PieceType::NPieces;
        Color color = Color::Empty;

        constexpr Piece() : type(PieceType::NPieces), color(Color::Empty){};
        constexpr Piece(PieceType _type, Color _color) :
            type(_type), color(_color){};

        /**
         * @brief Get the index of this piece
         *
         * @return constexpr int
         */
        constexpr inline int get_index() const {
            return color * PieceType::NPieces + type;
        };

        /**
         * @brief Get the material weight of this piece
         *
         * @return constexpr int Material weight
         */
        constexpr inline int get_weight() { return piece_weights[get_index()]; }

        /**
         * @brief Get the icon of this piece
         *
         * @return const char* Icon unicode character
         */
        inline const char *get_icon() const { return piece_icons[get_index()]; }

        /**
         * @brief Get the ASCII character of this piece
         *
         * @return const char ASCII code
         */
        inline const char get_char() const { return piece_chars[get_index()]; }

        /**
         * @brief Test if this piece is empty (invalid)
         *
         * @return true
         * @return false
         */
        constexpr inline bool is_empty() const { return color == Color::Empty; }
    };
} // namespace brainiac

#endif