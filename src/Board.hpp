#pragma once

#include <array>
#include <iostream>

#include "Bitboard.hpp"
#include "Move.hpp"
#include "Piece.hpp"

namespace Brainiac {
    /**
     * @brief Board representation.
     *
     */
    class Board {
        /**
         * @brief Positions of each of the 12 pieces on the board + 2 for black
         * and white pieces in general.
         *
         */
        std::array<Bitboard, 14> _bitboards;

        /**
         * @brief Mailbox for fast piece lookup.
         *
         */
        std::array<Piece, 64> _pieces;

      public:
        /**
         * @brief Get the bitboard array.
         *
         * @return const std::array<Bitboard, 14>&
         */
        const std::array<Bitboard, 14> &bitboards() const;

        /**
         * @brief Get the mailbox array of piecess.
         *
         * @return const std::array<Piece, 64>&
         */
        const std::array<Piece, 64> &pieces() const;

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
         * Set code page to display UTF16 characters (chcp 65001 on Powershell).
         *
         */
        void print() const;
    };
} // namespace Brainiac