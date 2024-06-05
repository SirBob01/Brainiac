#pragma once

#include <cstdint>
#include <vector>

#include "Hasher.hpp"
#include "Move.hpp"
#include "MoveList.hpp"
#include "Numeric.hpp"

namespace Brainiac {
    /**
     * @brief Represents discrete chronological game state. This allows backward
     * movement in time (undo moves).
     *
     * This stores information that is difficult to retrive during the
     * undo operation (e.g., castling rights).
     *
     */
    struct State {
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
         * @brief En-passant capture pawn.
         *
         */
        Square ep_pawn;

        /**
         * @brief Source square of castling move.
         *
         */
        Square castle_rook_src;

        /**
         * @brief Destination square of castling move.
         *
         */
        Square castle_rook_dst;

        /**
         * @brief Last captured piece. Empty in the case of en-passant.
         *
         */
        Piece dst_piece;

        /**
         * @brief Previous move.
         *
         */
        Move prev_move;

        /**
         * @brief Half-moves depend on the board state (pawn advances or
         * captures reset it)
         *
         */
        Clock halfmoves;

        /**
         * @brief Hash key.
         *
         */
        Hash hash;
    };
} // namespace Brainiac