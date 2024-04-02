#pragma once

#include <cstdint>
#include <random>
#include <vector>

#include "Board.hpp"
#include "MoveList.hpp"
#include "Piece.hpp"
#include "Utils.hpp"

namespace Brainiac {
    /**
     * @brief State hash bitfield.
     *
     */
    using StateHash = uint64_t;

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
     * @brief Initial random bitstrings for Zobrist hashing.
     *
     */
    static std::vector<StateHash> ZOBRIST_BITSTRINGS;

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
         * @brief Initialize state from a FEN string.
         *
         * @param fen
         */
        State(std::string fen);

        /**
         * @brief Get the Fen string of the board.
         *
         */
        std::string fen() const;

        /**
         * @brief Compute the Zobrist hash value.
         *
         */

        /**
         * @brief Compute the Zobrist hash value with a given seed for the
         * initialization of random bitstrings.
         *
         * Note that the seed is universal and will only be used once.
         * Subsequent calls to hash() for any state will not re-initialize the
         * table.
         *
         * @param seed
         * @return StateHash
         */
        StateHash hash(unsigned seed = time(0)) const;

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