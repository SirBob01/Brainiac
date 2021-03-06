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
        NPieces = 6,

        // Useful constant for bitboard manipulation
        NPieces2 = 2 * PieceType::NPieces
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
    class Piece {
        uint16_t _bitfield;

      public:
        constexpr Piece(PieceType type, Color color) :
            _bitfield(((color * PieceType::NPieces + type) << 8) |
                      (color << 4) | type){};
        constexpr Piece() : Piece(PieceType::NPieces, Color::Empty){};

        /**
         * @brief Get the type of the piece
         *
         * @return constexpr PieceType
         */
        constexpr inline PieceType get_type() const {
            return static_cast<PieceType>(_bitfield & 0xF);
        };

        /**
         * @brief Get the color of the piece
         *
         * @return constexpr Color
         */
        constexpr inline Color get_color() const {
            return static_cast<Color>((_bitfield >> 4) & 0xF);
        };

        /**
         * @brief Get the index of this piece
         *
         * @return constexpr int
         */
        constexpr inline int get_index() const { return _bitfield >> 8; };

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
        constexpr inline bool is_empty() const {
            return get_color() == Color::Empty;
        }
    };
} // namespace brainiac

#endif