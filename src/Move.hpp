#pragma once

#include <cassert>
#include <cstdint>
#include <string>

#include "Bitboard.hpp"
#include "Square.hpp"

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
     * @brief Chess move representation for the `src` and `dst` squares, as well
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
         * @param src
         * @param dst
         * @param type
         */
        Move(Square src, Square dst, MoveType type);
        Move();

        /**
         * @brief Get the starting square of this move.
         *
         * @return Square
         */
        Square src() const;

        /**
         * @brief Get the ending square of this move.
         *
         * @return Square
         */
        Square dst() const;

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