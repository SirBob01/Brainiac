#ifndef BRAINIAC_MOVE_H_
#define BRAINIAC_MOVE_H_

#include <cassert>
#include <string>

namespace brainiac {
    /**
     * A field storing move flag combinations
     */
    using MoveFlagSet = uint32_t;

    /**
     * A field storing castling flags
     */
    using CastlingFlagSet = uint8_t;

    /**
     * Flags that describe a chess move
     */
    enum MoveFlag : MoveFlagSet {
        Capture = 1,
        EnPassant = 1 << 1,
        PawnAdvance = 1 << 2,
        PawnDouble = 1 << 3,
        Castling = 1 << 4,
        KnightPromo = 1 << 5,
        QueenPromo = 1 << 6,
        BishopPromo = 1 << 7,
        RookPromo = 1 << 8
    };

    /**
     * The different castling types
     */
    enum Castle : CastlingFlagSet {
        WK = 1,
        WQ = 1 << 1,
        BK = 1 << 2,
        BQ = 1 << 3,
    };

    /**
     * Flags for promotions
     */
    constexpr MoveFlagSet promotions[4] = {
        MoveFlag::KnightPromo,
        MoveFlag::QueenPromo,
        MoveFlag::BishopPromo,
        MoveFlag::RookPromo,
    };

    /**
     * Castling rights grouped by color
     */
    constexpr Castle castling_rights[4] = {
        Castle::WK,
        Castle::WQ,

        Castle::BK,
        Castle::BQ,
    };

    /**
     * Pawn movement flags
     */
    constexpr MoveFlagSet pawn_single_flags = MoveFlag::PawnAdvance;
    constexpr MoveFlagSet pawn_double_flags =
        MoveFlag::PawnAdvance | MoveFlag::PawnDouble;
    constexpr MoveFlagSet pawn_capture_flags = MoveFlag::Capture;
    constexpr MoveFlagSet en_passant_flags =
        MoveFlag::Capture | MoveFlag::EnPassant;

    /**
     * The square is a cell index on the board (0-63)
     */
    using Square = int;

    /**
     * @brief Enum of all squares
     *
     */
    // clang-format off
    enum Squares : Square {
        A1, B1, C1, D1, E1, F1, G1, H1,
        A2, B2, C2, D2, E2, F2, G2, H2,
        A3, B3, C3, D3, E3, F3, G3, H3,
        A4, B4, C4, D4, E4, F4, G4, H4,
        A5, B5, C5, D5, E5, F5, G5, H5,
        A6, B6, C6, D6, E6, F6, G6, H6,
        A7, B7, C7, D7, E7, F7, G7, H7,
        A8, B8, C8, D8, E8, F8, G8, H8,
        InvalidSquare
    };
    // clang-format on

    /**
     * @brief Test if a square is invalid
     *
     * @return true
     * @return false
     */
    constexpr inline bool is_square_invalid(Square sq) {
        return sq == Squares::InvalidSquare;
    };

    /**
     * @brief Get the bitboard mask of a square
     *
     * @return constexpr uint64_t Bitmask
     */
    constexpr inline uint64_t get_square_mask(Square sq) { return 1ULL << sq; };

    /**
     * @brief Convert a square to a standard notation string
     *
     * @param sq Square
     * @return std::string Standard notation string
     */
    std::string square_to_string(Square sq);

    /**
     * @brief Convert a standard notation string to a square
     *
     * @param notation Standard notation string
     * @return Square  Square
     */
    Square string_to_square(std::string notation);

    /**
     * @brief Chess move representation for the `from` and `to` squares, as well
     * as associated move flags. This is stored efficiently in a single 32-bit
     * integer.
     *
     */
    class Move {
        uint32_t _bitfield;

      public:
        constexpr Move() :
            Move(Squares::InvalidSquare,
                 Squares::InvalidSquare,
                 0){}; // 0b10000001000000
        constexpr Move(Square from, Square to, MoveFlagSet flags) :
            _bitfield(to | (from << 7ULL) | (flags << 14ULL)){};

        /**
         * @brief Get the starting square of this move
         *
         * @return Square From square
         */
        constexpr inline Square get_from() const {
            return (_bitfield >> 7ULL) & 127ULL;
        };

        /**
         * @brief Get the ending square of this move
         *
         * @return Square To square
         */
        constexpr inline Square get_to() const { return _bitfield & 127ULL; };

        /**
         * @brief Get the flags of this move
         *
         * @return constexpr MoveFlagSet All move flags
         */
        constexpr inline MoveFlagSet get_flags() const {
            return _bitfield >> 14ULL;
        };

        /**
         * @brief Test if a move is invalid
         *
         * @return true
         * @return false
         */
        constexpr inline bool is_invalid() const {
            return _bitfield == 8256ULL;
        };

        /**
         * @brief Test equality with another move
         *
         * @param other Other move
         * @return true
         * @return false
         */
        constexpr inline bool operator==(const Move &other) const {
            return _bitfield == other._bitfield;
        };

        /**
         * @brief Get the standard notation string of this move
         *
         * @return std::string Standard notation string
         */
        inline std::string standard_notation() const {
            Square from = get_from();
            Square to = get_to();
            MoveFlagSet flags = get_flags();

            std::string s = square_to_string(from) + square_to_string(to);
            if (flags & MoveFlag::RookPromo) s += 'r';
            if (flags & MoveFlag::BishopPromo) s += 'b';
            if (flags & MoveFlag::KnightPromo) s += 'n';
            if (flags & MoveFlag::QueenPromo) s += 'q';
            return s;
        };
    };
} // namespace brainiac

#endif