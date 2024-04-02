#pragma once

#include <cassert>
#include <cstdint>
#include <string>

#include "Bitboard.hpp"

namespace Brainiac {
    /**
     * @brief Underlying bitfield for castling rights.
     *
     */
    using CastlingFlagSet = uint8_t;

    /**
     * @brief Castling rights.
     *
     */
    enum CastlingRight : CastlingFlagSet {
        WK,
        WQ,
        BK,
        BQ,
    };

    /**
     * @brief Enum of all squares.
     *
     */
    // clang-format off
    enum Square : uint8_t {
        A1, B1, C1, D1, E1, F1, G1, H1,
        A2, B2, C2, D2, E2, F2, G2, H2,
        A3, B3, C3, D3, E3, F3, G3, H3,
        A4, B4, C4, D4, E4, F4, G4, H4,
        A5, B5, C5, D5, E5, F5, G5, H5,
        A6, B6, C6, D6, E6, F6, G6, H6,
        A7, B7, C7, D7, E7, F7, G7, H7,
        A8, B8, C8, D8, E8, F8, G8, H8,
        Null,
    };
    // clang-format on

    /**
     * Enum of all types of moves.
     */
    enum MoveType : uint8_t {
        Quiet,
        PawnDouble,
        KingCastle,
        QueenCastle,
        Capture,
        EnPassant,
        KnightPromo,
        RookPromo,
        BishopPromo,
        QueenPromo,
        KnightPromoCapture,
        RookPromoCapture,
        BishopPromoCapture,
        QueenPromoCapture,
    };

    /**
     * @brief Get the bitboard mask of a square.
     *
     * @return Bitboard
     */
    Bitboard get_square_mask(Square sq);

    /**
     * @brief Convert a square to a standard notation string.
     *
     * @param sq
     * @return std::string
     */
    std::string square_to_string(Square sq);

    /**
     * @brief Convert a standard notation string to a square.
     *
     * @param standard_notation
     * @return Square
     */
    Square string_to_square(std::string standard_notation);

    /**
     * @brief Chess move representation for the `from` and `to` squares, as well
     * as the move type. This is stored compactly as a single 16-bit
     * integer.
     *
     */
    class Move {
        uint16_t _bitfield;

      public:
        /**
         * @brief Create a Move.
         *
         * @param from
         * @param to
         * @param type
         */
        Move(Square from, Square to, MoveType type);
        Move();

        /**
         * @brief Get the starting square of this move.
         *
         * @return Square From square
         */
        Square from() const;

        /**
         * @brief Get the ending square of this move.
         *
         * @return Square To square
         */
        Square to() const;

        /**
         * @brief Get the type of this move.
         *
         * @return MoveType
         */
        MoveType type() const;

        /**
         * @brief Test equality with another move.
         *
         * @param other
         * @return true
         * @return false
         */
        bool operator==(const Move &other) const;

        /**
         * @brief Get the standard notation string of this move.
         *
         * @return std::string
         */
        std::string standard_notation() const;
    };
} // namespace Brainiac