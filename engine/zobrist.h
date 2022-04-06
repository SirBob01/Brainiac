#ifndef CHESS_ZOBRIST_H_
#define CHESS_ZOBRIST_H_

#include <chrono>
#include <random>
#include <vector>

#include "bits.h"
#include "move.h"
#include "piece.h"

namespace chess {
    /**
     * Bitstrings
     */
    inline std::vector<uint64_t> piece_bitstrings;
    inline std::vector<uint64_t> castling_bitstrings;
    inline std::vector<uint64_t> en_passant_bitstrings;
    inline uint64_t turn_bitstring;

    /**
     * Inialize the Zobrist bitstrings
     */
    void zobrist_init();

    /**
     * Generate a Zobrist hash for a given board
     */
    uint64_t zobrist_hash(Color turn,
                          uint64_t *bitboards,
                          uint8_t castling_rights,
                          Square en_passant);

    /**
     * Get the bitstring associated with a piece at a square
     */
    uint64_t zobrist_bitstring(Piece piece, Square square);
} // namespace chess

#endif