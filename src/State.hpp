#pragma once

#include <cstdint>
#include <vector>

#include "Bitboard.hpp"
#include "Board.hpp"
#include "Hasher.hpp"
#include "Move.hpp"
#include "MoveGen.hpp"
#include "MoveList.hpp"
#include "Piece.hpp"
#include "Sliders.hpp"
#include "Utils.hpp"

namespace Brainiac {
    /**
     * @brief Represents discrete chronological game state. This allows forward
     * and backward movement in time (undo/redo moves).
     *
     * This stores information that is difficult to retrive during the
     * undo operation (e.g., castling rights).
     *
     */
    struct State {
        /**
         * @brief Board state.
         *
         */
        Board board;

        /**
         * @brief Castling rights.
         *
         */
        CastlingFlagSet castling;

        /**
         * @brief En-passant destination square.
         *
         */
        Square ep_dst;

        /**
         * @brief Current turn.
         *
         */
        Color turn;

        /**
         * @brief Half-moves depend on the board state (pawn advances or
         * captures reset it)
         *
         */
        unsigned halfmoves;

        /**
         * @brief Fullmove counter.
         *
         */
        unsigned fullmoves;

        /**
         * @brief Is king in check?
         *
         */
        bool check;

        /**
         * @brief Move set.
         *
         */
        MoveList moves;

        /**
         * @brief Hash value.
         *
         */
        Hash hash;

        /**
         * @brief Initialize an empty state.
         *
         */
        State();

        /**
         * @brief Initialize state from a FEN string.
         *
         * @param fen
         * @param hasher
         */
        State(std::string fen, Hasher &hasher);

        /**
         * @brief Get the Fen string of the board.
         *
         */
        std::string fen() const;

        /**
         * @brief Pretty print the state.
         *
         */
        void print() const;

        /**
         * @brief Generate the moves for the specified turn.
         *
         */
        void generate_moves();
    };
} // namespace Brainiac