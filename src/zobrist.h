#ifndef BRAINIAC_ZOBRIST_H_
#define BRAINIAC_ZOBRIST_H_

#include <chrono>
#include <random>
#include <vector>

#include "bits.h"
#include "move.h"
#include "piece.h"

namespace brainiac {
    /**
     * @brief Zobrist bitstrings
     *
     */
    inline std::vector<uint64_t> piece_bitstrings;
    inline std::vector<uint64_t> castling_bitstrings;
    inline std::vector<uint64_t> en_passant_bitstrings;
    inline uint64_t turn_bitstring;

    /**
     * @brief Inialize the Zobrist bitstrings
     *
     */
    void zobrist_init();

    /**
     * @brief Generate a zobrist hash for a board
     *
     * @param turn Current turn
     * @param bitboards Array of bitboards
     * @param castling_rights Castling rights
     * @param en_passant En passant square
     * @return uint64_t Hash
     */
    uint64_t zobrist_hash(Color turn,
                          uint64_t *bitboards,
                          CastlingFlagSet castling_rights,
                          const Square en_passant);

    /**
     * @brief Get the zobrist bitstring associated with a piece at a square
     *
     * @param piece
     * @param square
     * @return uint64_t
     */
    inline uint64_t zobrist_bitstring(const Piece &piece, const Square square) {
        return piece_bitstrings[(square * PieceType::NPieces2) +
                                piece.get_index()];
    }
} // namespace brainiac

#endif