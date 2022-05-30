#ifndef BRAINIAC_BOARD_H_
#define BRAINIAC_BOARD_H_

#include <array>
#include <iostream>
#include <string>

#include "bits.h"
#include "move.h"
#include "piece.h"
#include "util.h"
#include "zobrist.h"

namespace brainiac {
    /**
     * @brief A bitboard is an occupancy array for each of the 64
     * squares on the chess board. This is conveniently represented as a 64-bit
     * integer to allow fast calculation of positional algorithms, such as move
     * generation and attack vectors.
     *
     */
    using Bitboard = uint64_t;

    /**
     * @brief A stack-allocated list of moves
     *
     */
    class MoveList {
        // Theoretical maximum number of moves
        std::array<Move, 321> _list;
        int _count = 0;

      public:
        /**
         * @brief Access a move in the list
         *
         * @param index
         * @return Move
         */
        inline Move operator[](int index) const { return _list[index]; };

        /**
         * @brief Get the size of the list
         *
         * @return int
         */
        inline int size() const { return _count; };

        /**
         * @brief Clear the list
         *
         */
        inline void clear() { _count = 0; };

        /**
         * @brief Add a new move to the list
         *
         * @param move
         */
        inline void add(Square from, Square to, MoveFlagSet flags) {
            _list[_count++] = Move(from, to, flags);
        };

        /**
         * @brief Get the pointer to the first move in the list
         *
         * @return Move*
         */
        inline Move *begin() { return _list.data(); };

        /**
         * @brief Get the pointer to after the end of the list
         *
         * @return Move*
         */
        inline Move *end() { return _list.data() + _count; };
    };

    /**
     * @brief Represents chronological board state. This allows forward and
     * backward movement in time (undo/redo moves).
     *
     * Stores information that can be difficult to retrive during the undo
     * operation (e.g., castling rights)
     *
     */
    struct BoardState {
        // Represent the positions of each of the 12 pieces on the board
        // Extra 2 bitboards represent white/black pieces in general
        Bitboard _bitboards[14] = {0};

        // qkQK (from most to least significant bit)
        CastlingFlagSet _castling_rights = 0;
        Square _en_passant_target = Squares::InvalidSquare;

        // Unlike fullmoves, halfmoves depends on state of the board (pawn
        // advance or capture reset)
        int _halfmoves;

        uint64_t _hash;
        bool _check = false;

        MoveList _moves;
    };

    /**
     * @brief Chess board simulation
     *
     */
    class Board {
        std::vector<BoardState> _states;
        int _current_state;

        Color _turn;
        int _fullmoves;

        /**
         * @brief Generate all legal moves
         * If move list is empty, then player is in checkmate
         *
         */
        void generate_moves();

        /**
         * @brief Clone and push the current board state for the next turn
         *
         * @return BoardState&
         */
        BoardState &push_state();

      public:
        Board(std::string fen_string =
                  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        Board(Board &other) : Board(other.generate_fen()){};

        /**
         * @brief Generate a FEN string of the current state for serialization
         *
         * @return std::string
         */
        std::string generate_fen();

        /**
         * @brief Get all legal moves available to the current player
         *
         */
        inline MoveList &get_moves() { return _states[_current_state]._moves; }

        /**
         * @brief Get a piece on a square
         *
         * @param sq
         * @return Piece
         */
        Piece get_at(const Square sq);

        /**
         * @brief Set a piece on a square
         *
         * @param sq
         * @param piece
         */
        inline void set_at(const Square sq, const Piece &piece) {
            BoardState &state = _states[_current_state];
            Bitboard mask = get_square_mask(sq);
            state._bitboards[PieceType::NPieces2 + piece.color] |= mask;
            state._bitboards[piece.get_index()] |= mask;
        }

        /**
         * @brief Get a piece on the board by coordinates
         *
         * @param row
         * @param col
         * @return Piece
         */
        inline Piece get_at_coords(int row, int col) {
            return get_at(row * 8 + col);
        }

        /**
         * @brief Set a piece on the board by coordinates
         *
         * @param row
         * @param col
         * @param piece
         */
        inline void set_at_coords(int row, int col, const Piece &piece) {
            set_at(row * 8 + col, piece);
        }

        /**
         * @brief Clear the square of a piece bitboard
         *
         * @param sq
         */
        inline void clear_at(const Square sq, const Piece &piece) {
            BoardState &state = _states[_current_state];
            Bitboard mask = ~get_square_mask(sq);
            state._bitboards[PieceType::NPieces2 + piece.color] &= mask;
            state._bitboards[piece.get_index()] &= mask;
        }

        /**
         * @brief Perform a null move, skipping the current turn
         *
         */
        void skip_move();

        /**
         * @brief Make a move and update internal state
         * This assumes the move is legal
         *
         * @param move
         */
        void make_move(const Move &move);

        /**
         * @brief Return the board to the previous state
         * Only perform if board is not in the initial state
         *
         */
        inline void undo_move() {
            _current_state--;
            _turn = static_cast<Color>(!_turn);
            if (_turn == Color::Black) {
                _fullmoves--;
            }
        }

        /**
         * @brief Move the board state forward in time
         * Only perform if board is not in the latest state
         *
         */
        inline void redo_move() {
            _current_state++;
            if (_turn == Color::Black) {
                _fullmoves++;
            }
            _turn = static_cast<Color>(!_turn);
        }

        /**
         * @brief Tests if the board is already in the initial state (cannot
         * undo)
         *
         * @return true
         * @return false
         */
        inline bool is_initial() { return _current_state == 0; }

        /**
         * @brief Tests if the board is already in the latest state (cannot
         * redo)
         *
         * @return true
         * @return false
         */
        inline bool is_latest() { return _current_state == _states.size() - 1; }

        /**
         * @brief Test if a move is legal
         *
         * @param move
         * @return true
         * @return false
         */
        inline bool is_legal(Move &move) {
            const MoveList &moves = get_moves();
            for (int i = 0; i < moves.size(); i++) {
                if (moves[i] == move) {
                    return true;
                }
            }
            return false;
        }

        /**
         * @brief Tests if the current player's king is in check
         *
         * @return true
         * @return false
         */
        inline bool is_check() { return _states[_current_state]._check; }

        /**
         * @brief Tests if the current player is in checkmate (lost)
         *
         * @return true
         * @return false
         */
        inline bool is_checkmate() {
            return get_moves().size() == 0 && is_check();
        }

        /**
         * @brief Tests if the game is a stalemate
         *
         * @return true
         * @return false
         */
        inline bool is_stalemate() {
            return get_moves().size() == 0 && !is_check();
        }

        /**
         * @brief Tests if the game is a draw
         *
         * @return true
         * @return false
         */
        inline bool is_draw() {
            // If there are only 2 pieces left (both kings), then it is a draw
            BoardState &state = _states[_current_state];
            Bitboard all = state._bitboards[12] | state._bitboards[13];
            int rem = count_set_bits(all);
            return is_stalemate() || state._halfmoves >= 100 || rem == 2;
        }

        /**
         * @brief Generate a valid chess move from the given from-to square
         * pairings
         *
         * Used primarily to validate user-submitted moves
         *
         * @param from
         * @param to
         * @param promotion
         * @return Move
         */
        Move
        create_move(const Square from, const Square to, char promotion = 0);

        /**
         * @brief Generate a valid chess move given a standard notation string
         *
         * @param standard_notation Valid standard notation string
         * @return Move
         */
        Move create_move(std::string standard_notation);

        /**
         * @brief Get the current number of halfmoves to enforce the 50-move
         * rule
         *
         */
        inline int get_halfmoves() {
            return _states[_current_state]._halfmoves;
        }

        /**
         * @brief Return the bitfield corresponding to castling rights
         *
         */
        inline uint8_t get_castling_rights() {
            return _states[_current_state]._castling_rights;
        }

        /**
         * @brief Get the current turn (either White or Black)
         *
         */
        inline Color get_turn() { return _turn; }

        /**
         * @brief Get the Zobrist hash of the board
         *
         */
        inline uint64_t get_hash() { return _states[_current_state]._hash; }

        /**
         * @brief Get the bitboard associated with a piece
         *
         * @param piece
         * @return Bitboard
         */
        inline Bitboard &get_bitboard(Piece &piece) {
            return _states[_current_state]._bitboards[piece.get_index()];
        }

        /**
         * @brief Get the bitboard associated with a piece
         *
         * @param type
         * @param color
         * @return Bitboard
         */
        inline Bitboard &get_bitboard(PieceType type, Color color) {
            return _states[_current_state]
                ._bitboards[color * PieceType::NPieces + type];
        }

        /**
         * @brief Get the bitboard associated with a color
         *
         * @param color
         * @return Bitboard
         */
        inline Bitboard &get_bitboard(Color color) {
            return _states[_current_state]
                ._bitboards[PieceType::NPieces2 + color];
        }

        /**
         * @brief Get a bitboard by idex
         *
         * @param index
         * @return Bitboard
         */
        inline Bitboard &get_bitboard(int index) {
            return _states[_current_state]._bitboards[index];
        }

        /**
         * @brief Print the board on the console
         *
         */
        void print();
    };
} // namespace brainiac

#endif