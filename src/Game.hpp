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
        Hash _hash;

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
         * @brief Get the move list for the current turn.
         *
         * @return const MoveList&
         */
        const MoveList &moves() const;

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
         * @brief Pretty print the current game state.
         *
         */
        void print() const;
    };
} // namespace Brainiac