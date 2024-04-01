#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "State.hpp"
#include "Utils.hpp"

namespace Brainiac {
    static std::string DEFAULT_BOARD_FEN =
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    /**
     * @brief Game simulation.
     *
     */
    class Game {
        std::vector<State> _states;
        unsigned _state_index;
        unsigned _fullmoves;
        Color _turn;

        /**
         * @brief Clone and push the current board state for the next turn.
         *
         * @return State&
         */
        State &push_state();

        /**
         * @brief Generate moves for the current state.
         *
         */
        void generate_moves();

      public:
        Game(std::string fen = DEFAULT_BOARD_FEN);

        /**
         * @brief Get the fen string of the current board state.
         *
         * @return std::string
         */
        std::string fen() const;

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
        void make_move(Move move);

        /**
         * @brief Undo the last move made.
         *
         */
        void undo_move();

        /**
         * @brief Skip the current turn.
         *
         */
        void skip_move();

        /**
         * @brief Pretty print the current board state.
         *
         */
        void print() const;
    };
} // namespace Brainiac