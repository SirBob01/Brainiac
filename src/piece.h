#ifndef BRAINIAC_PIECE_H_
#define BRAINIAC_PIECE_H_

#include <cassert>

namespace brainiac {
    enum PieceType {
        King = 0,
        Pawn = 1,
        Rook = 2,
        Knight = 3,
        Bishop = 4,
        Queen = 5,
        NPieces = 6
    };

    enum Color { White = 0, Black = 1, Empty = 2 };

    static const char *PieceChars = "KPRNBQkprnbq";
    static const char *PieceDisplay[] = {"\u2654",
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
     * Calculate the material score of the board state
     */
    constexpr int piece_weights[] = {
        4,
        1,
        5,
        3,
        3,
        9, // White pieces
        -4,
        -1,
        -5,
        -3,
        -3,
        -9 // Black pieces
    };

    /**
     * Each piece uniquely indexes a bitboard
     */
    struct Piece {
        // Default empty square
        PieceType type = PieceType::NPieces;
        Color color = Color::Empty;

        inline int get_piece_index() const {
            return PieceType::NPieces * color + type;
        };

        inline int get_color_index() const {
            return PieceType::NPieces * 2 + color;
        };

        inline const char *get_display() const {
            return PieceDisplay[get_piece_index()];
        }

        inline const char get_char() const {
            return PieceChars[get_piece_index()];
        }

        inline bool is_empty() const { return color == Color::Empty; }
    };
} // namespace brainiac

#endif