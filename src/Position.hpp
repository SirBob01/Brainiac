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
     * @brief Game position simulation.
     *
     */
    class Position {
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
        Position(std::string fen = DEFAULT_BOARD_FEN, Hasher hasher = Hasher());

        /**
         * @brief Get the FEN string of the current game state.
         *
         * @param include_counters
         * @return std::string
         */
        std::string fen(bool include_counters = true) const;

        /**
         * @brief Get the hash of the current game state.
         *
         * @return StateHash
         */
        Hash hash() const;

        /**
         * @brief Get the current board state.
         *
         * @return const Board&
         */
        const Board &board() const;

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
        bool is_stalemate() const;

        /**
         * @brief Test if the game is a draw.
         *
         * @return true
         * @return false
         */
        bool is_draw() const;

        /**
         * @brief Test if the position is the initial state.
         *
         * @return true
         * @return false
         */
        bool is_start() const;

        /**
         * @brief Test if the position is the latest state.
         *
         * @return true
         * @return false
         */
        bool is_end() const;

        /**
         * @brief Test if the position is quiet.
         *
         * @return true
         * @return false
         */
        bool is_quiet();

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
         * @brief Redo the next move.
         *
         */
        void redo();

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