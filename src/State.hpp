#pragma once

#include <cstdint>

#include "Board.hpp"
#include "MoveList.hpp"
#include "Piece.hpp"

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
    enum CastlingRights : CastlingFlagSet {
        WK = 1,
        WQ = 1 << 1,
        BK = 1 << 2,
        BQ = 1 << 3,
    };

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
         * @brief En-passant target square.
         *
         */
        Square ep_target;

        /**
         * @brief Half-moves depend on the board state (pawn advances or
         * captures reset it)
         *
         */
        unsigned halfmoves;

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
         * @brief Zobrist hash value.
         *
         */
        uint64_t hash;

        /**
         * @brief Force compute Zobrist hash value.
         *
         */
        void compute_hash();

        /**
         * @brief Move generator for the specified turn.
         *
         * @param turn
         */
        void generate_moves(Color turn);
    };
} // namespace Brainiac