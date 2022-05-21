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
    constexpr CastlingFlagSet color_castling_rights[2] = {
        Castle::WK | Castle::WQ,
        Castle::BK | Castle::BQ,
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
    using Square = uint32_t;

    /**
     * @brief Test if a square is invalid
     *
     * @return true
     * @return false
     */
    constexpr inline bool is_square_invalid(Square sq) { return sq == 64; };

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
     * Representation of a chess move
     */
    class Move {
        uint32_t _bitfield;

      public:
        Move() : _bitfield(8256){}; // 0b10000001000000
        Move(Square from, Square to, MoveFlagSet flags) {
            _bitfield = to | (from << 7) | (flags << 14);
        };

        /**
         * @brief Get the starting square of this move
         *
         * @return Square From square
         */
        constexpr inline Square get_from() const {
            return (_bitfield >> 7) & 63;
        };

        /**
         * @brief Get the ending square of this move
         *
         * @return Square To square
         */
        constexpr inline Square get_to() const { return _bitfield & 63; };

        /**
         * @brief Get the flags of this move
         *
         * @return constexpr MoveFlagSet All move flags
         */
        constexpr inline MoveFlagSet get_flags() const {
            return _bitfield >> 14;
        };

        /**
         * @brief Test if a move is invalid
         *
         * @return true
         * @return false
         */
        constexpr inline bool is_invalid() const { return _bitfield == 8256; };

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