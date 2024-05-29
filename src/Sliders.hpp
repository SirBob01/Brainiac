#pragma once

#include <array>

#include "Bitboard.hpp"
#include "Move.hpp"
#include "Piece.hpp"

namespace Brainiac {
    /**
     * @brief Stores necessary information for fetching the moveset
     * of a sliding piece using magic bitboards.
     *
     */
    struct SlidingMoveTable {
        unsigned shift;
        Bitboard block_mask;
        Bitboard magic;
        std::array<Bitboard, 4096> move_masks;
    };

    /**
     * @brief Magic numbers for rook move masks.
     *
     */
    constexpr std::array<Bitboard, 64> ROOK_MAGICS = {
        0x0a8002c000108020, 0x06c00049b0002001, 0x0100200010090040,
        0x2480041000800801, 0x0280028004000800, 0x0900410008040022,
        0x0280020001001080, 0x2880002041000080, 0xa000800080400034,
        0x0004808020004000, 0x2290802004801000, 0x0411000d00100020,
        0x0402800800040080, 0x000b000401004208, 0x2409000100040200,
        0x0001002100004082, 0x0022878001e24000, 0x1090810021004010,
        0x0801030040200012, 0x0500808008001000, 0x0a08018014000880,
        0x8000808004000200, 0x0201008080010200, 0x0801020000441091,
        0x0000800080204005, 0x1040200040100048, 0x0000120200402082,
        0x0d14880480100080, 0x0012040280080080, 0x0100040080020080,
        0x9020010080800200, 0x0813241200148449, 0x0491604001800080,
        0x0100401000402001, 0x4820010021001040, 0x0400402202000812,
        0x0209009005000802, 0x0810800601800400, 0x4301083214000150,
        0x204026458e001401, 0x0040204000808000, 0x8001008040010020,
        0x8410820820420010, 0x1003001000090020, 0x0804040008008080,
        0x0012000810020004, 0x1000100200040208, 0x430000a044020001,
        0x0280009023410300, 0x00e0100040002240, 0x0000200100401700,
        0x2244100408008080, 0x0008000400801980, 0x0002000810040200,
        0x8010100228810400, 0x2000009044210200, 0x4080008040102101,
        0x0040002080411d01, 0x2005524060000901, 0x0502001008400422,
        0x489a000810200402, 0x0001004400080a13, 0x4000011008020084,
        0x0026002114058042,
    };

    /**
     * @brief Magic numbers for bishop move masks.
     *
     */
    constexpr std::array<Bitboard, 64> BISHOP_MAGICS = {
        0x89a1121896040240, 0x2004844802002010, 0x2068080051921000,
        0x62880a0220200808, 0x0004042004000000, 0x0100822020200011,
        0xc00444222012000a, 0x0028808801216001, 0x0400492088408100,
        0x0201c401040c0084, 0x00840800910a0010, 0x0000082080240060,
        0x2000840504006000, 0x30010c4108405004, 0x1008005410080802,
        0x8144042209100900, 0x0208081020014400, 0x004800201208ca00,
        0x0f18140408012008, 0x1004002802102001, 0x0841000820080811,
        0x0040200200a42008, 0x0000800054042000, 0x88010400410c9000,
        0x0520040470104290, 0x1004040051500081, 0x2002081833080021,
        0x000400c00c010142, 0x941408200c002000, 0x0658810000806011,
        0x0188071040440a00, 0x4800404002011c00, 0x0104442040404200,
        0x0511080202091021, 0x0004022401120400, 0x80c0040400080120,
        0x8040010040820802, 0x0480810700020090, 0x0102008e00040242,
        0x0809005202050100, 0x8002024220104080, 0x0431008804142000,
        0x0019001802081400, 0x0200014208040080, 0x3308082008200100,
        0x041010500040c020, 0x4012020c04210308, 0x208220a202004080,
        0x0111040120082000, 0x6803040141280a00, 0x2101004202410000,
        0x8200000041108022, 0x0000021082088000, 0x0002410204010040,
        0x0040100400809000, 0x0822088220820214, 0x0040808090012004,
        0x00910224040218c9, 0x0402814422015008, 0x0090014004842410,
        0x0001000042304105, 0x0010008830412a00, 0x2520081090008908,
        0x40102000a0a60140,
    };

    /**
     * @brief For sliding pieces and a set of blockers, find the valid
     * movement bits. Only works for positive rays, must reverse the bits to
     * work with negative rays.
     *
     * @param bitboard
     * @param occupied
     * @return constexpr Bitboard
     */
    constexpr Bitboard get_ray_attack_mask(Bitboard bitboard,
                                           Bitboard occupied) {
        return occupied ^ (occupied - 2 * bitboard);
    }

    /**
     * @brief Generate an occupancy bitboard from a given index.
     *
     * @param index
     * @param bitcount
     * @param mask
     * @return constexpr Bitboard
     */
    constexpr Bitboard
    get_occupancy(unsigned index, unsigned bitcount, Bitboard mask) {
        Bitboard occupancy = 0;
        for (unsigned i = 0; i < bitcount; i++) {
            unsigned shift = find_lsb_bitboard(mask);
            mask &= (mask - 1ULL);
            if (index & (1ULL << i)) {
                occupancy |= (1ULL << shift);
            }
        }
        return occupancy;
    }

    constexpr Bitboard get_n_mask(Bitboard bitboard, Bitboard occupied) {
        unsigned shift = find_lsb_bitboard(bitboard);
        Bitboard file_mask = FILES[0] << (7 & shift);
        return get_ray_attack_mask(bitboard, occupied & file_mask) & file_mask;
    }

    constexpr Bitboard get_s_mask(Bitboard bitboard, Bitboard occupied) {
        unsigned shift = find_lsb_bitboard(bitboard);
        Bitboard file_mask = FILES[0] << (7 & shift);
        return flip_vertical_bitboard(
            get_ray_attack_mask(flip_vertical_bitboard(bitboard),
                                flip_vertical_bitboard(occupied & file_mask)) &
            file_mask);
    }

    constexpr Bitboard get_e_mask(Bitboard bitboard, Bitboard occupied) {
        unsigned shift = find_lsb_bitboard(bitboard);
        Bitboard rank_mask = RANKS[7] >> (56 - 8 * (shift / 8));
        return get_ray_attack_mask(bitboard, occupied & rank_mask) & rank_mask;
    }

    constexpr Bitboard get_w_mask(Bitboard bitboard, Bitboard occupied) {
        unsigned shift = find_lsb_bitboard(bitboard);
        Bitboard rank_mask = RANKS[7] >> (56 - 8 * (shift / 8));
        return flip_horizontal_bitboard(
            get_ray_attack_mask(
                flip_horizontal_bitboard(bitboard),
                flip_horizontal_bitboard(occupied & rank_mask)) &
            rank_mask);
    }

    constexpr Bitboard get_ne_mask(Bitboard bitboard, Bitboard occupied) {
        unsigned shift = find_lsb_bitboard(bitboard);
        Bitboard diagonal_mask = SQUARE_DIAGONALS[shift];
        return get_ray_attack_mask(bitboard, occupied & diagonal_mask) &
               diagonal_mask;
    }

    constexpr Bitboard get_sw_mask(Bitboard bitboard, Bitboard occupied) {
        unsigned shift = find_lsb_bitboard(bitboard);
        Bitboard diagonal_mask = SQUARE_DIAGONALS[shift];
        return flip_vertical_bitboard(
            get_ray_attack_mask(
                flip_vertical_bitboard(bitboard),
                flip_vertical_bitboard(occupied & diagonal_mask)) &
            flip_vertical_bitboard(diagonal_mask));
    }

    constexpr Bitboard get_nw_mask(Bitboard bitboard, Bitboard occupied) {
        unsigned shift = find_lsb_bitboard(bitboard);
        Bitboard antidiag_mask = SQUARE_ANTI_DIAGONALS[shift];
        return get_ray_attack_mask(bitboard, occupied & antidiag_mask) &
               antidiag_mask;
    }

    constexpr Bitboard get_se_mask(Bitboard bitboard, Bitboard occupied) {
        unsigned shift = find_lsb_bitboard(bitboard);
        Bitboard antidiag_mask = SQUARE_ANTI_DIAGONALS[shift];
        return flip_vertical_bitboard(
            get_ray_attack_mask(
                flip_vertical_bitboard(bitboard),
                flip_vertical_bitboard(occupied & antidiag_mask)) &
            flip_vertical_bitboard(antidiag_mask));
    }

    constexpr std::array<SlidingMoveTable, 64> init_rook_tables() {
        std::array<SlidingMoveTable, 64> result = {};
        for (unsigned i = 0; i < 64; i++) {
            Bitboard bitboard = 1ULL << i;

            Bitboard rank_mask = SQUARE_RANKS[i] & ~(FILES[0] | FILES[7]);
            Bitboard file_mask = SQUARE_FILES[i] & ~(RANKS[0] | RANKS[7]);
            Bitboard block_mask = ~bitboard & (rank_mask | file_mask);

            SlidingMoveTable &table = result[i];
            table.block_mask = block_mask;
            table.shift = count_set_bitboard(block_mask);
            table.magic = ROOK_MAGICS[i];

            unsigned count = (1 << table.shift);
            for (unsigned j = 0; j < count; j++) {
                Bitboard blockers = get_occupancy(j, table.shift, block_mask);
                Bitboard index = (blockers * table.magic) >> (64 - table.shift);
                table.move_masks[index] = get_n_mask(bitboard, blockers) |
                                          get_s_mask(bitboard, blockers) |
                                          get_e_mask(bitboard, blockers) |
                                          get_w_mask(bitboard, blockers);
            }
        }
        return result;
    }

    constexpr std::array<SlidingMoveTable, 64> init_bishop_tables() {
        std::array<SlidingMoveTable, 64> result = {};
        for (int i = 0; i < 64; i++) {
            Bitboard bitboard = 1ULL << i;

            Bitboard diagonal_mask = SQUARE_DIAGONALS[i];
            Bitboard antidiag_mask = SQUARE_ANTI_DIAGONALS[i];
            Bitboard end_ranks_mask = RANKS[0] | RANKS[7];
            Bitboard end_files_mask = FILES[0] | FILES[7];

            Bitboard block_mask = diagonal_mask | antidiag_mask;
            block_mask &= ~(bitboard | end_ranks_mask | end_files_mask);

            SlidingMoveTable &table = result[i];
            table.block_mask = block_mask;
            table.shift = count_set_bitboard(block_mask);
            table.magic = BISHOP_MAGICS[i];

            unsigned count = (1 << table.shift);
            for (unsigned j = 0; j < count; j++) {
                Bitboard blockers = get_occupancy(j, table.shift, block_mask);
                Bitboard index = (blockers * table.magic) >> (64 - table.shift);
                table.move_masks[index] = get_ne_mask(bitboard, blockers) |
                                          get_se_mask(bitboard, blockers) |
                                          get_sw_mask(bitboard, blockers) |
                                          get_nw_mask(bitboard, blockers);
            }
        }
        return result;
    }

    /**
     * @brief Attack tables for sliding pieces.
     *
     */
    extern std::array<SlidingMoveTable, 64> ROOK_ATTACK_TABLES;
    extern std::array<SlidingMoveTable, 64> BISHOP_ATTACK_TABLES;
} // namespace Brainiac