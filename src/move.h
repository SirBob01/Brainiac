#ifndef BRAINIAC_MOVE_H_
#define BRAINIAC_MOVE_H_

#include <cassert>
#include <string>

namespace brainiac {
    /**
     * A position on the board represented by a bitshift value (0 - 63)
     */
    struct Square {
        int shift = -1;

        Square() : shift(-1){};
        Square(int shift) : shift(shift){};
        Square(char file, char rank);
        Square(std::string notation);

        inline std::string standard_notation() const {
            assert(!is_invalid());
            int row = shift / 8;
            int col = shift % 8;
            char rank = row + '1';
            char field = col + 'a';
            return std::string({field, rank});
        };

        constexpr inline uint64_t get_mask() const { return (1ULL << shift); };

        constexpr inline bool is_invalid() const { return shift == -1; }

        inline bool operator==(Square other) const {
            return shift == other.shift;
        }
    };

    /**
     * Flags that describe a chess move
     */
    enum MoveFlag {
        Capture = 1,
        EnPassant = 1 << 1,
        PawnAdvance = 1 << 2,
        PawnDouble = 1 << 3,
        Castling = 1 << 4,
        KnightPromo = 1 << 5,
        QueenPromo = 1 << 6,
        BishopPromo = 1 << 7,
        RookPromo = 1 << 8,
        Invalid = 1 << 9
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
     * Flags for promotions
     */
    constexpr uint64_t promotions[4] = {MoveFlag::KnightPromo,
                                        MoveFlag::QueenPromo,
                                        MoveFlag::BishopPromo,
                                        MoveFlag::RookPromo};

    /**
     * Castling rights grouped by color
     */
    constexpr uint64_t color_castling_rights[2] = {Castle::WK | Castle::WQ,
                                                   Castle::BK | Castle::BQ};

    /**
     * Pawn movement flags
     */
    constexpr uint64_t pawn_single_flags = MoveFlag::PawnAdvance;
    constexpr uint64_t pawn_double_flags =
        MoveFlag::PawnAdvance | MoveFlag::PawnDouble;
    constexpr uint64_t pawn_capture_flags = MoveFlag::Capture;
    constexpr uint64_t en_passant_flags =
        MoveFlag::Capture | MoveFlag::EnPassant;

    /**
     * Container of a chess move
     */
    struct Move {
        Square from;
        Square to;
        uint64_t flags = MoveFlag::Invalid;

        constexpr inline bool is_invalid() const {
            return flags & MoveFlag::Invalid;
        };

        inline bool operator==(const Move &other) {
            return from == other.from && to == other.to && flags == other.flags;
        }

        inline std::string standard_notation() const {
            std::string s = from.standard_notation() + to.standard_notation();
            if (flags & MoveFlag::RookPromo) s += 'r';
            if (flags & MoveFlag::BishopPromo) s += 'b';
            if (flags & MoveFlag::KnightPromo) s += 'n';
            if (flags & MoveFlag::QueenPromo) s += 'q';
            return s;
        }
    };
} // namespace brainiac

#endif