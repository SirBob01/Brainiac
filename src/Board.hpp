#pragma once

#include <iostream>

#include "Bitboard.hpp"
#include "Move.hpp"
#include "Piece.hpp"

namespace Brainiac {
    /**
     * @brief Board representation.
     *
     */
    struct Board {
        /**
         * @brief Positions of each of the 12 pieces on the board + 2 for black
         * and white pieces in general.
         *
         */
        Bitboard _bitboards[14];

        /**
         * @brief Mailbox for fast piece lookup.
         *
         */
        Piece _pieces[64];

        /**
         * @brief Get the piece at a given square.
         *
         * @param sq
         * @return Piece
         */
        Piece get(Square sq) const;

        /**
         * @brief Set the piece at a given square.
         *
         * @param sq
         * @param piece
         */
        void set(Square sq, Piece piece);

        /**
         * @brief Clear the piece at a given square.
         *
         * @param sq
         * @param piece
         */
        void clear(Square sq);

        /**
         * @brief Pretty print the board pieces.
         *
         */
        void print() const;
    };
} // namespace Brainiac