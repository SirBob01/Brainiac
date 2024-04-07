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
     * @brief Table of king moves by bit index.
     *
     */
    constexpr std::array<Bitboard, 64> KING_MOVE_MASKS = {
        0x0000000000000302, 0x0000000000000705, 0x0000000000000e0a,
        0x0000000000001c14, 0x0000000000003828, 0x0000000000007050,
        0x000000000000e0a0, 0x000000000000c040, 0x0000000000030203,
        0x0000000000070507, 0x00000000000e0a0e, 0x00000000001c141c,
        0x0000000000382838, 0x0000000000705070, 0x0000000000e0a0e0,
        0x0000000000c040c0, 0x0000000003020300, 0x0000000007050700,
        0x000000000e0a0e00, 0x000000001c141c00, 0x0000000038283800,
        0x0000000070507000, 0x00000000e0a0e000, 0x00000000c040c000,
        0x0000000302030000, 0x0000000705070000, 0x0000000e0a0e0000,
        0x0000001c141c0000, 0x0000003828380000, 0x0000007050700000,
        0x000000e0a0e00000, 0x000000c040c00000, 0x0000030203000000,
        0x0000070507000000, 0x00000e0a0e000000, 0x00001c141c000000,
        0x0000382838000000, 0x0000705070000000, 0x0000e0a0e0000000,
        0x0000c040c0000000, 0x0003020300000000, 0x0007050700000000,
        0x000e0a0e00000000, 0x001c141c00000000, 0x0038283800000000,
        0x0070507000000000, 0x00e0a0e000000000, 0x00c040c000000000,
        0x0302030000000000, 0x0705070000000000, 0x0e0a0e0000000000,
        0x1c141c0000000000, 0x3828380000000000, 0x7050700000000000,
        0xe0a0e00000000000, 0xc040c00000000000, 0x0203000000000000,
        0x0507000000000000, 0x0a0e000000000000, 0x141c000000000000,
        0x2838000000000000, 0x5070000000000000, 0xa0e0000000000000,
        0x40c0000000000000,
    };

    /**
     * @brief Table of knight moves by bit index.
     *
     */
    constexpr std::array<Bitboard, 64> KNIGHT_MOVE_MASKS = {
        0x0000000000020400, 0x0000000000050800, 0x00000000000a1100,
        0x0000000000142200, 0x0000000000284400, 0x0000000000508800,
        0x0000000000a01000, 0x0000000000402000, 0x0000000002040004,
        0x0000000005080008, 0x000000000a110011, 0x0000000014220022,
        0x0000000028440044, 0x0000000050880088, 0x00000000a0100010,
        0x0000000040200020, 0x0000000204000402, 0x0000000508000805,
        0x0000000a1100110a, 0x0000001422002214, 0x0000002844004428,
        0x0000005088008850, 0x000000a0100010a0, 0x0000004020002040,
        0x0000020400040200, 0x0000050800080500, 0x00000a1100110a00,
        0x0000142200221400, 0x0000284400442800, 0x0000508800885000,
        0x0000a0100010a000, 0x0000402000204000, 0x0002040004020000,
        0x0005080008050000, 0x000a1100110a0000, 0x0014220022140000,
        0x0028440044280000, 0x0050880088500000, 0x00a0100010a00000,
        0x0040200020400000, 0x0204000402000000, 0x0508000805000000,
        0x0a1100110a000000, 0x1422002214000000, 0x2844004428000000,
        0x5088008850000000, 0xa0100010a0000000, 0x4020002040000000,
        0x0400040200000000, 0x0800080500000000, 0x1100110a00000000,
        0x2200221400000000, 0x4400442800000000, 0x8800885000000000,
        0x100010a000000000, 0x2000204000000000, 0x0004020000000000,
        0x0008050000000000, 0x00110a0000000000, 0x0022140000000000,
        0x0044280000000000, 0x0088500000000000, 0x0010a00000000000,
        0x0020400000000000,
    };

    /**
     * @brief Magic numbers for rook move masks.
     *
     */
    constexpr std::array<Bitboard, 64> ROOK_MAGICS = {
        0xa8002c000108020,  0x6c00049b0002001,  0x100200010090040,
        0x2480041000800801, 0x280028004000800,  0x900410008040022,
        0x280020001001080,  0x2880002041000080, 0xa000800080400034,
        0x4808020004000,    0x2290802004801000, 0x411000d00100020,
        0x402800800040080,  0xb000401004208,    0x2409000100040200,
        0x1002100004082,    0x22878001e24000,   0x1090810021004010,
        0x801030040200012,  0x500808008001000,  0xa08018014000880,
        0x8000808004000200, 0x201008080010200,  0x801020000441091,
        0x800080204005,     0x1040200040100048, 0x120200402082,
        0xd14880480100080,  0x12040280080080,   0x100040080020080,
        0x9020010080800200, 0x813241200148449,  0x491604001800080,
        0x100401000402001,  0x4820010021001040, 0x400402202000812,
        0x209009005000802,  0x810800601800400,  0x4301083214000150,
        0x204026458e001401, 0x40204000808000,   0x8001008040010020,
        0x8410820820420010, 0x1003001000090020, 0x804040008008080,
        0x12000810020004,   0x1000100200040208, 0x430000a044020001,
        0x280009023410300,  0xe0100040002240,   0x200100401700,
        0x2244100408008080, 0x8000400801980,    0x2000810040200,
        0x8010100228810400, 0x2000009044210200, 0x4080008040102101,
        0x40002080411d01,   0x2005524060000901, 0x502001008400422,
        0x489a000810200402, 0x1004400080a13,    0x4000011008020084,
        0x26002114058042,
    };

    /**
     * @brief Magic numbers for bishop move masks.
     *
     */
    constexpr std::array<Bitboard, 64> BISHOP_MAGICS = {
        0x89a1121896040240, 0x2004844802002010, 0x2068080051921000,
        0x62880a0220200808, 0x4042004000000,    0x100822020200011,
        0xc00444222012000a, 0x28808801216001,   0x400492088408100,
        0x201c401040c0084,  0x840800910a0010,   0x82080240060,
        0x2000840504006000, 0x30010c4108405004, 0x1008005410080802,
        0x8144042209100900, 0x208081020014400,  0x4800201208ca00,
        0xf18140408012008,  0x1004002802102001, 0x841000820080811,
        0x40200200a42008,   0x800054042000,     0x88010400410c9000,
        0x520040470104290,  0x1004040051500081, 0x2002081833080021,
        0x400c00c010142,    0x941408200c002000, 0x658810000806011,
        0x188071040440a00,  0x4800404002011c00, 0x104442040404200,
        0x511080202091021,  0x4022401120400,    0x80c0040400080120,
        0x8040010040820802, 0x480810700020090,  0x102008e00040242,
        0x809005202050100,  0x8002024220104080, 0x431008804142000,
        0x19001802081400,   0x200014208040080,  0x3308082008200100,
        0x41010500040c020,  0x4012020c04210308, 0x208220a202004080,
        0x111040120082000,  0x6803040141280a00, 0x2101004202410000,
        0x8200000041108022, 0x21082088000,      0x2410204010040,
        0x40100400809000,   0x822088220820214,  0x40808090012004,
        0x910224040218c9,   0x402814422015008,  0x90014004842410,
        0x1000042304105,    0x10008830412a00,   0x2520081090008908,
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

            for (unsigned j = 0; j < (1 << table.shift); j++) {
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

            for (unsigned j = 0; j < (1 << table.shift); j++) {
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
    constexpr std::array<SlidingMoveTable, 64> ROOK_ATTACK_TABLES =
        init_rook_tables();
    constexpr std::array<SlidingMoveTable, 64> BISHOP_ATTACK_TABLES =
        init_bishop_tables();
} // namespace Brainiac