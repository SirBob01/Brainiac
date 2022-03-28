#ifndef CHESS_MOVE_H_
#define CHESS_MOVE_H_

#include <cassert>
#include <string>

namespace chess {
    /**
     * A position on the board represented by a bitshift value (0 - 63)
     */
    struct Square {
        int shift = -1;

        Square() : shift(-1){};
        Square(int shift) : shift(shift){};
        Square(char file, char rank);
        Square(std::string notation);

        inline std::string standard_notation() {
            assert(!is_invalid());
            int row = shift / 8;
            int col = shift % 8;
            char rank = row + '1';
            char field = col + 'a';
            return std::string({field, rank});
        };

        inline uint64_t get_mask() { return (1ULL << shift); };

        inline bool is_invalid() { return shift == -1; }

        inline bool operator==(Square other) { return shift == other.shift; }
    };

    /**
     * Flags that describe a chess move
     */
    enum MoveFlag {
        Quiet = 1,
        Capture = 1 << 1,
        EnPassant = 1 << 2,
        PawnAdvance = 1 << 3,
        PawnDouble = 1 << 4,
        Castling = 1 << 5,
        KnightPromo = 1 << 6,
        QueenPromo = 1 << 7,
        BishopPromo = 1 << 8,
        RookPromo = 1 << 9,
        Invalid = 1 << 10
    };

    /**
     * The different castling types
     */
    enum Castle {
        WK = 1,
        WQ = 1 << 1,
        BK = 1 << 2,
        BQ = 1 << 3,
    };

    /**
     * Container of a chess move
     */
    struct Move {
        Square from;
        Square to;
        unsigned flags = MoveFlag::Invalid;

        inline bool is_invalid() { return flags & MoveFlag::Invalid; };

        inline std::string standard_notation() {
            std::string s = from.standard_notation() + to.standard_notation();
            if (flags & MoveFlag::RookPromo)
                s += 'r';
            if (flags & MoveFlag::BishopPromo)
                s += 'b';
            if (flags & MoveFlag::KnightPromo)
                s += 'n';
            if (flags & MoveFlag::QueenPromo)
                s += 'q';
            return s;
        }
    };
} // namespace chess

#endif