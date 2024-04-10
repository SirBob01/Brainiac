#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "Hasher.hpp"
#include "State.hpp"

namespace Brainiac {
    static const std::string DEFAULT_BOARD_FEN =
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    /**
     * @brief Game simulation.
     *
     */
    class Game {
        std::vector<State> _states;
        unsigned _index;
        Hasher _hasher;

        /**
         * @brief Push a new state onto the array, overwriting any states ahead
         * of the curernt index for the `undo` case.
         *
         * @return State&
         */
        State &push_state();

      public:
        Game(std::string fen = DEFAULT_BOARD_FEN, Hasher hasher = Hasher());

        /**
         * @brief Get the fen string of the current game state.
         *
         * @return std::string
         */
        std::string fen() const;

        /**
         * @brief Get the hash of the current game state.
         *
         * @return StateHash
         */
        Hash hash() const;

        /**
         * @brief Get the current board state.
         *
         * @return const Board
         */
        const Board board() const;

        /**
         * @brief Get the current turn.
         *
         * @return Color
         */
        Color turn() const;

        /**
         * @brief Get the move list for the current turn.
         *
         * @return const MoveList&
         */
        const MoveList &moves() const;

        /**
         * @brief Get the current set of castling rights.
         *
         * @return const CastlingFlagSet
         */
        const CastlingFlagSet castling() const;

        /**
         * @brief Get the current halfmove clock.
         *
         * @return unsigned
         */
        unsigned halfmoves() const;

        /**
         * @brief Get the current fullmove clock.
         *
         * @return unsigned
         */
        unsigned fullmoves() const;

        /**
         * @brief Test if the king is in check for the current turn.
         *
         * @return true
         * @return false
         */
        bool is_check() const;

        /**
         * @brief Test if the king is in checkmate for the current turn.
         *
         * @return true
         * @return false
         */
        bool is_checkmate() const;

        /**
         * @brief Test if the game is a stalemate.
         *
         * @return true
         * @return false
         */
        bool is_statelmate() const;

        /**
         * @brief Make a move. This assumes moves are legal.
         *
         * @param move
         */
        void make(Move move);

        /**
         * @brief Undo the last move made.
         *
         */
        void undo();

        /**
         * @brief Skip the current turn.
         *
         */
        void skip();

        /**
         * @brief Find a legal move from given src and dst square pairings.
         *
         * Used primarily to validate user-submitted moves.
         *
         * @param src
         * @param dst
         * @param promotion
         * @return Move
         */
        Move
        find_move(const Square src, const Square dst, char promotion = 0) const;

        /**
         * @brief Pretty print the current game state.
         *
         */
        void print() const;
    };
} // namespace Brainiac