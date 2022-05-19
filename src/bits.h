#ifndef BRAINIAC_BITS_H_
#define BRAINIAC_BITS_H_

#ifdef _WIN32

#include <stdlib.h>
#define bswap_64(x) _byteswap_uint64(x)

#elif defined(__APPLE__)

#include <libkern/OSByteOrder.h>
#define bswap_64(x) OSSwapInt64(x)

#else

#include <byteswap.h>

#endif

#include <bitset>
#include <cstdint>
#include <iostream>
#include <stack>

#include "move.h"
#include "piece.h"

namespace brainiac {
    /**
     * Cardinal and ordinal directions on the board
     */
    enum Direction {
        DownRight = 0,
        Right = 1,
        UpRight = 2,
        Up = 3,
        UpLeft = 4,
        Left = 5,
        DownLeft = 6,
        Down = 7
    };

    /**
     * Stores necessary information for fetching the moveset
     * of a sliding piece using magic bitboards
     */
    struct SlidingMoveTable {
        int shift;
        uint64_t block_mask;
        uint64_t magic;
        uint64_t move_masks[1ULL << 12];
    };

    /**
     * Values for calculating adjacent positions using bitwise operations
     */
    constexpr int direction_shift[8] = {9, 1, -7, -8, -9, -1, 7, 8};

    /**
     * Constant bitmasks to eliminate overflowing bits during shift operations
     */
    constexpr uint64_t wrap_bitmasks[8] = {
        0xfefefefefefefe00,
        0xfefefefefefefefe,
        0x00fefefefefefefe,
        0x00ffffffffffffff,
        0x007f7f7f7f7f7f7f,
        0x7f7f7f7f7f7f7f7f,
        0x7f7f7f7f7f7f7f00,
        0xffffffffffffff00,
    };

    /**
     * Bit scan table for fetching the index of the least significant bit
     */
    constexpr uint64_t debruijn64 = 0x07EDD5E59A4E28C2;
    constexpr int bitscan_table[64] = {
        63, 0,  58, 1,  59, 47, 53, 2,  60, 39, 48, 27, 54, 33, 42, 3,
        61, 51, 37, 40, 49, 18, 28, 20, 55, 30, 34, 11, 43, 14, 22, 4,
        62, 57, 46, 52, 38, 26, 32, 41, 50, 36, 17, 19, 29, 10, 13, 21,
        56, 45, 25, 31, 35, 16, 9,  12, 44, 24, 15, 8,  23, 7,  6,  5};

    /**
     * Horizontal bitboard flip constants
     */
    constexpr uint64_t horflip_k[3] = {0x5555555555555555,
                                       0x3333333333333333,
                                       0x0f0f0f0f0f0f0f0f};

    /**
     * Bitmask constants
     */
    constexpr uint64_t rank1 = 0x00000000000000FF;
    constexpr uint64_t rank2 = 0x000000000000FF00;
    constexpr uint64_t rank3 = 0x0000000000FF0000;
    constexpr uint64_t rank4 = 0x00000000FF000000;
    constexpr uint64_t rank5 = 0x000000FF00000000;
    constexpr uint64_t rank6 = 0x0000FF0000000000;
    constexpr uint64_t rank7 = 0x00FF000000000000;
    constexpr uint64_t rank8 = 0xFF00000000000000;

    constexpr uint64_t fileA = 0x0101010101010101;
    constexpr uint64_t fileB = 0x0202020202020202;
    constexpr uint64_t fileC = 0x0404040404040404;
    constexpr uint64_t fileD = 0x0808080808080808;
    constexpr uint64_t fileE = 0x1010101010101010;
    constexpr uint64_t fileF = 0x2020202020202020;
    constexpr uint64_t fileG = 0x4040404040404040;
    constexpr uint64_t fileH = 0x8080808080808080;

    constexpr uint64_t ranks[8] =
        {rank1, rank2, rank3, rank4, rank5, rank6, rank7, rank8};
    constexpr uint64_t files[8] =
        {fileA, fileB, fileC, fileD, fileE, fileF, fileG, fileH};

    constexpr uint64_t main_diagonal = 0x8040201008040201;
    constexpr uint64_t anti_diagnoal = 0x0102040810204080;

    constexpr uint64_t end_ranks = rank1 | rank8;
    constexpr uint64_t end_files = fileA | fileH;

    /**
     * Move mapping for sliding pieces
     */
    extern SlidingMoveTable rook_attack_tables[64];
    extern SlidingMoveTable bishop_attack_tables[64];
    extern uint64_t king_move_masks[64];
    extern uint64_t knight_move_masks[64];

    /**
     * Pre-calculated magic number constants
     */
    constexpr uint64_t rook_magics[64] = {
        0xa8002c000108020ULL,  0x6c00049b0002001ULL,  0x100200010090040ULL,
        0x2480041000800801ULL, 0x280028004000800ULL,  0x900410008040022ULL,
        0x280020001001080ULL,  0x2880002041000080ULL, 0xa000800080400034ULL,
        0x4808020004000ULL,    0x2290802004801000ULL, 0x411000d00100020ULL,
        0x402800800040080ULL,  0xb000401004208ULL,    0x2409000100040200ULL,
        0x1002100004082ULL,    0x22878001e24000ULL,   0x1090810021004010ULL,
        0x801030040200012ULL,  0x500808008001000ULL,  0xa08018014000880ULL,
        0x8000808004000200ULL, 0x201008080010200ULL,  0x801020000441091ULL,
        0x800080204005ULL,     0x1040200040100048ULL, 0x120200402082ULL,
        0xd14880480100080ULL,  0x12040280080080ULL,   0x100040080020080ULL,
        0x9020010080800200ULL, 0x813241200148449ULL,  0x491604001800080ULL,
        0x100401000402001ULL,  0x4820010021001040ULL, 0x400402202000812ULL,
        0x209009005000802ULL,  0x810800601800400ULL,  0x4301083214000150ULL,
        0x204026458e001401ULL, 0x40204000808000ULL,   0x8001008040010020ULL,
        0x8410820820420010ULL, 0x1003001000090020ULL, 0x804040008008080ULL,
        0x12000810020004ULL,   0x1000100200040208ULL, 0x430000a044020001ULL,
        0x280009023410300ULL,  0xe0100040002240ULL,   0x200100401700ULL,
        0x2244100408008080ULL, 0x8000400801980ULL,    0x2000810040200ULL,
        0x8010100228810400ULL, 0x2000009044210200ULL, 0x4080008040102101ULL,
        0x40002080411d01ULL,   0x2005524060000901ULL, 0x502001008400422ULL,
        0x489a000810200402ULL, 0x1004400080a13ULL,    0x4000011008020084ULL,
        0x26002114058042ULL,
    };
    constexpr uint64_t bishop_magics[64] = {
        0x89a1121896040240ULL, 0x2004844802002010ULL, 0x2068080051921000ULL,
        0x62880a0220200808ULL, 0x4042004000000ULL,    0x100822020200011ULL,
        0xc00444222012000aULL, 0x28808801216001ULL,   0x400492088408100ULL,
        0x201c401040c0084ULL,  0x840800910a0010ULL,   0x82080240060ULL,
        0x2000840504006000ULL, 0x30010c4108405004ULL, 0x1008005410080802ULL,
        0x8144042209100900ULL, 0x208081020014400ULL,  0x4800201208ca00ULL,
        0xf18140408012008ULL,  0x1004002802102001ULL, 0x841000820080811ULL,
        0x40200200a42008ULL,   0x800054042000ULL,     0x88010400410c9000ULL,
        0x520040470104290ULL,  0x1004040051500081ULL, 0x2002081833080021ULL,
        0x400c00c010142ULL,    0x941408200c002000ULL, 0x658810000806011ULL,
        0x188071040440a00ULL,  0x4800404002011c00ULL, 0x104442040404200ULL,
        0x511080202091021ULL,  0x4022401120400ULL,    0x80c0040400080120ULL,
        0x8040010040820802ULL, 0x480810700020090ULL,  0x102008e00040242ULL,
        0x809005202050100ULL,  0x8002024220104080ULL, 0x431008804142000ULL,
        0x19001802081400ULL,   0x200014208040080ULL,  0x3308082008200100ULL,
        0x41010500040c020ULL,  0x4012020c04210308ULL, 0x208220a202004080ULL,
        0x111040120082000ULL,  0x6803040141280a00ULL, 0x2101004202410000ULL,
        0x8200000041108022ULL, 0x21082088000ULL,      0x2410204010040ULL,
        0x40100400809000ULL,   0x822088220820214ULL,  0x40808090012004ULL,
        0x910224040218c9ULL,   0x402814422015008ULL,  0x90014004842410ULL,
        0x1000042304105ULL,    0x10008830412a00ULL,   0x2520081090008908ULL,
        0x40102000a0a60140ULL,
    };

    /**
     * Print a bitboard (8 bits per row)
     */
    void print_bitboard(uint64_t bitboard);

    /**
     * Count the number of set bits on a bitboard
     */
    int count_set_bits(uint64_t bitboard);

    /**
     * Vertically flip the bitboard
     */
    inline uint64_t flip_vertical(uint64_t bitboard) {
        return bswap_64(bitboard);
    }

    /**
     * Horizontally flip the bitboard
     */
    constexpr inline uint64_t flip_horizontal(uint64_t bitboard) {
        bitboard =
            ((bitboard >> 1) & horflip_k[0]) + 2 * (bitboard & horflip_k[0]);
        bitboard =
            ((bitboard >> 2) & horflip_k[1]) + 4 * (bitboard & horflip_k[1]);
        bitboard =
            ((bitboard >> 4) & horflip_k[2]) + 16 * (bitboard & horflip_k[2]);
        return bitboard;
    }

    /**
     * Calculates the index of the least significant bit in the binary string
     * Use this to iterate through all set bits on a bitboard (active pieces)
     */
    constexpr inline int find_lsb(uint64_t binary) {
        return bitscan_table[((binary & -binary) * debruijn64) >> 58];
    }

    /**
     * Get the diagonal from the current bit position
     */
    constexpr inline uint64_t get_diagonal_mask(int shift) {
        int diag = 8 * (shift & 7) - (shift & 56);
        int nort = -diag & (diag >> 31);
        int sout = diag & (-diag >> 31);
        return (main_diagonal >> sout) << nort;
    }

    /**
     * Get the antidiagonal from the current bit position
     */
    constexpr inline uint64_t get_antidiag_mask(int shift) {
        int diag = 56 - 8 * (shift & 7) - (shift & 56);
        int nort = -diag & (diag >> 31);
        int sout = diag & (-diag >> 31);
        return (anti_diagnoal >> sout) << nort;
    }

    /**
     * Shift a binary string by an arbitrary amount, negative shifts are right
     * shifts
     */
    constexpr inline uint64_t shift(uint64_t binary, int shift) {
        return shift < 0 ? binary >> -shift : binary << shift;
    }

    /**
     * Return a new bitboard with a point adjacent to the binary string in a
     * specific direction
     */
    constexpr inline uint64_t get_adjacent(uint64_t bitboard, Direction dir) {
        return shift(bitboard, direction_shift[dir]) & wrap_bitmasks[dir];
    }

    /**
     * For sliding pieces and a set of blockers, find the valid movement bits
     * Only works for positive rays, must reverse the bits to work with negative
     * rays
     */
    constexpr inline uint64_t get_ray_attack(uint64_t bitboard,
                                             uint64_t occupied) {
        return occupied ^ (occupied - 2 * bitboard);
    }

    /**
     * Generate a rook move set on-the-fly
     *
     * This is slow, so it is used for generating the rook attack tables
     */
    inline uint64_t get_rook_mask_otf(uint64_t bitboard, uint64_t occupied) {
        const int shift = find_lsb(bitboard);
        const uint64_t rank_mask = rank8 >> (56 - 8 * (shift / 8));
        const uint64_t file_mask = fileA << (7 & shift);

        uint64_t rank_positive =
            get_ray_attack(bitboard, occupied & rank_mask) & rank_mask;
        uint64_t rank_negative =
            get_ray_attack(flip_horizontal(bitboard),
                           flip_horizontal(occupied & rank_mask)) &
            rank_mask;

        uint64_t file_positive =
            get_ray_attack(bitboard, occupied & file_mask) & file_mask;
        uint64_t file_negative =
            get_ray_attack(flip_vertical(bitboard),
                           flip_vertical(occupied & file_mask)) &
            file_mask;
        return (rank_positive | flip_horizontal(rank_negative) | file_positive |
                flip_vertical(file_negative));
    }

    /**
     * Generate a bishop move set on-the-fly
     *
     * This is slow, so it is used for generating the bishop attack tables
     */
    inline uint64_t get_bishop_mask_otf(uint64_t bitboard, uint64_t occupied) {
        int shift = find_lsb(bitboard);
        uint64_t diagonal_mask = get_diagonal_mask(shift);
        uint64_t antidiag_mask = get_antidiag_mask(shift);

        uint64_t diagonal_positive =
            get_ray_attack(bitboard, occupied & diagonal_mask) & diagonal_mask;
        uint64_t diagonal_negative =
            get_ray_attack(flip_vertical(bitboard),
                           flip_vertical(occupied & diagonal_mask)) &
            flip_vertical(diagonal_mask);

        uint64_t antidiag_positive =
            get_ray_attack(bitboard, occupied & antidiag_mask) & antidiag_mask;
        uint64_t antidiag_negative =
            get_ray_attack(flip_vertical(bitboard),
                           flip_vertical(occupied & antidiag_mask)) &
            flip_vertical(antidiag_mask);
        return (diagonal_positive | flip_vertical(diagonal_negative) |
                antidiag_positive | flip_vertical(antidiag_negative));
    }

    /**
     * Get the all possible directions the king can move to from its current
     * position
     */
    inline uint64_t get_king_mask(uint64_t bitboard) {
        return king_move_masks[find_lsb(bitboard)];
    }

    /**
     * Get the all possible directions the knight can move to from its current
     * position
     */
    inline uint64_t get_knight_mask(uint64_t bitboard) {
        uint64_t mask = 0;
        while (bitboard) {
            mask |= knight_move_masks[find_lsb(bitboard)];
            bitboard &= (bitboard - 1);
        }
        return mask;
    }

    /**
     * Get the position of the pawn after advancing a single rank
     */
    constexpr inline uint64_t
    get_pawn_advance_mask(uint64_t bitboard, uint64_t all_pieces, Color color) {
        return (color == Color::White)
                   ? get_adjacent(bitboard, Direction::Down) & ~all_pieces
                   : get_adjacent(bitboard, Direction::Up) & ~all_pieces;
    }

    /**
     * Get the position of the pawn after advancing 2 ranks
     */
    constexpr inline uint64_t
    get_pawn_double_mask(uint64_t bitboard, uint64_t all_pieces, Color color) {
        // Move twice, assuming both cells are clear
        // Only move if target square is rank 4 or rank 5
        uint64_t advance = get_pawn_advance_mask(
            get_pawn_advance_mask(bitboard, all_pieces, color),
            all_pieces,
            color);
        return (color == Color::White) ? advance & rank4 : advance & rank5;
    }

    /**
     * Get the all possible positions of the pawn if capturing (either en
     * passant or regular)
     */
    constexpr inline uint64_t get_pawn_capture_mask(uint64_t bitboard,
                                                    Color color) {
        return (color == Color::White)
                   ? get_adjacent(bitboard, Direction::DownLeft) |
                         get_adjacent(bitboard, Direction::DownRight)
                   : get_adjacent(bitboard, Direction::UpLeft) |
                         get_adjacent(bitboard, Direction::UpRight);
    }

    /**
     * Get all possible moves for the rook
     */
    constexpr inline uint64_t
    get_rook_mask(uint64_t bitboard, uint64_t allies, uint64_t enemies) {
        const int square = find_lsb(bitboard);
        const SlidingMoveTable &table = rook_attack_tables[square];
        const uint64_t blockers = table.block_mask & (allies | enemies);
        const uint64_t index = (blockers * table.magic) >> (64 - table.shift);
        return table.move_masks[index] & ~allies;
    }

    /**
     * Get all possible moves for the bishop
     */
    constexpr inline uint64_t
    get_bishop_mask(uint64_t bitboard, uint64_t allies, uint64_t enemies) {
        const int square = find_lsb(bitboard);
        const SlidingMoveTable &table = bishop_attack_tables[square];
        const uint64_t blockers = table.block_mask & (allies | enemies);
        const uint64_t index = (blockers * table.magic) >> (64 - table.shift);
        return table.move_masks[index] & ~allies;
    }

    /**
     * Get all possible moves for the queen
     * Simply perform bitwise OR on the rook and bishop masks
     */
    constexpr inline uint64_t
    get_queen_mask(uint64_t bitboard, uint64_t allies, uint64_t enemies) {
        const int square = find_lsb(bitboard);

        const SlidingMoveTable &rook_table = rook_attack_tables[square];
        const uint64_t rook_blockers =
            rook_table.block_mask & (allies | enemies);
        const uint64_t rook_index =
            (rook_blockers * rook_table.magic) >> (64 - rook_table.shift);
        const uint64_t rook_mask = rook_table.move_masks[rook_index];

        const SlidingMoveTable &bishop_table = bishop_attack_tables[square];
        const uint64_t bishop_blockers =
            bishop_table.block_mask & (allies | enemies);
        const uint64_t bishop_index =
            (bishop_blockers * bishop_table.magic) >> (64 - bishop_table.shift);
        const uint64_t bishop_mask = bishop_table.move_masks[bishop_index];

        return (rook_mask | bishop_mask) & ~allies;
    }

    /**
     * Get the final positions for the king when castling
     */
    constexpr inline uint64_t get_castling_mask(uint64_t all_pieces,
                                                Castle side) {
        // BC and FG files must be clear on the end ranks
        switch (side) {
        case Castle::WK:
            if (all_pieces & 0x60) return 0;
            return 0x40;
            break;
        case Castle::WQ:
            if (all_pieces & 0xE) return 0;
            return 0x4;
            break;
        case Castle::BK:
            if (all_pieces & 0x6000000000000000) return 0;
            return 0x4000000000000000;
            break;
        case Castle::BQ:
            if (all_pieces & 0x0E00000000000000) return 0;
            return 0x0400000000000000;
            break;
        }
        return 0;
    }

    /**
     * Generate an occupancy bitboard from a given index
     */
    constexpr inline uint64_t
    generate_occupancy(int index, int bit_count, uint64_t mask) {
        uint64_t occupancy = 0;
        for (int i = 0; i < bit_count; i++) {
            int shift = find_lsb(mask);
            mask &= (mask - 1ULL);
            if (index & (1ULL << i)) {
                occupancy |= (1ULL << shift);
            }
        }
        return occupancy;
    }

    /**
     * Initialize the rook attack tables
     */
    constexpr inline void init_rook_tables() {
        for (int i = 0; i < 64; i++) {
            uint64_t bitboard = 1ULL << i;

            uint64_t rank_mask = rank8 >> (56 - 8 * (i / 8));
            uint64_t file_mask = fileA << (7 & i);

            rank_mask &= ~end_files;
            file_mask &= ~end_ranks;

            uint64_t block_mask = ~bitboard & (rank_mask | file_mask);

            SlidingMoveTable &table = rook_attack_tables[i];
            table.block_mask = block_mask;
            table.shift = count_set_bits(block_mask);
            table.magic = rook_magics[i];

            for (int j = 0; j < (1 << table.shift); j++) {
                uint64_t blockers =
                    generate_occupancy(j, table.shift, block_mask);
                uint64_t index = (blockers * table.magic) >> (64 - table.shift);
                table.move_masks[index] = get_rook_mask_otf(bitboard, blockers);
            }
        }
    }

    /**
     * Initialize the bishop attack tables
     */
    constexpr inline void init_bishop_tables() {
        for (int i = 0; i < 64; i++) {
            uint64_t bitboard = 1ULL << i;

            uint64_t diagonal_mask = get_diagonal_mask(i);
            uint64_t antidiag_mask = get_antidiag_mask(i);

            uint64_t block_mask = diagonal_mask | antidiag_mask;
            block_mask &= ~(bitboard | end_ranks | end_files);

            SlidingMoveTable &table = bishop_attack_tables[i];
            table.block_mask = block_mask;
            table.shift = count_set_bits(block_mask);
            table.magic = bishop_magics[i];

            for (int j = 0; j < (1 << table.shift); j++) {
                uint64_t blockers =
                    generate_occupancy(j, table.shift, block_mask);
                uint64_t index = (blockers * table.magic) >> (64 - table.shift);
                table.move_masks[index] =
                    get_bishop_mask_otf(bitboard, blockers);
            }
        }
    }

    /**
     * Initialize the movement masks for the king
     */
    constexpr inline void init_king_tables() {
        for (int i = 0; i < 64; i++) {
            uint64_t bitboard = 1ULL << i;
            king_move_masks[i] = get_adjacent(bitboard, Direction::Left) |
                                 get_adjacent(bitboard, Direction::Right) |
                                 get_adjacent(bitboard, Direction::Up) |
                                 get_adjacent(bitboard, Direction::Down) |
                                 get_adjacent(bitboard, Direction::UpLeft) |
                                 get_adjacent(bitboard, Direction::UpRight) |
                                 get_adjacent(bitboard, Direction::DownLeft) |
                                 get_adjacent(bitboard, Direction::DownRight);
        }
    }

    /**
     * Initialize the movement masks for the knight
     */
    constexpr inline void init_knight_tables() {
        for (int i = 0; i < 64; i++) {
            uint64_t bitboard = 1ULL << i;
            knight_move_masks[i] =
                get_adjacent(get_adjacent(bitboard, Direction::UpLeft),
                             Direction::Left) |
                get_adjacent(get_adjacent(bitboard, Direction::DownLeft),
                             Direction::Left) |
                get_adjacent(get_adjacent(bitboard, Direction::UpRight),
                             Direction::Right) |
                get_adjacent(get_adjacent(bitboard, Direction::DownRight),
                             Direction::Right) |
                get_adjacent(get_adjacent(bitboard, Direction::UpLeft),
                             Direction::Up) |
                get_adjacent(get_adjacent(bitboard, Direction::UpRight),
                             Direction::Up) |
                get_adjacent(get_adjacent(bitboard, Direction::DownLeft),
                             Direction::Down) |
                get_adjacent(get_adjacent(bitboard, Direction::DownRight),
                             Direction::Down);
        }
    }
} // namespace brainiac

#endif