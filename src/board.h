#ifndef BRAINIAC_BOARD_H_
#define BRAINIAC_BOARD_H_

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

        // Bitboard representing the attackers on each square (excluding king)
        Bitboard _attackers = 0;
        std::vector<Move> _legal_moves;

        uint64_t _hash;
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
         * @brief Generate single-step moves
         *
         * @param bitboard
         * @param is_king
         * @param mask_func
         */
        void generate_step_moves(Bitboard bitboard,
                                 bool is_king,
                                 Bitboard (*mask_func)(Bitboard));

        /**
         * @brief Generate slider moves
         *
         * @param bitboard
         * @param mask_func
         */
        void generate_slider_moves(Bitboard bitboard,
                                   Bitboard (*mask_func)(Bitboard,
                                                         Bitboard,
                                                         Bitboard));

        /**
         * @brief Generate pawn moves
         *
         * Pawns have special cases (ugh.)
         *
         * @param bitboard
         */
        void generate_pawn_moves(Bitboard bitboard);

        /**
         * @brief Generate castling moves
         *
         * @param bitboard
         */
        void generate_castling_moves(Bitboard bitboard);

        /**
         * @brief Test if a move is legal
         *
         * - If king is the moving piece, make sure destination square is not an
         * attack target
         * - If move is an en passant, king must not currently be in check
         * - If non-king piece, it must not be pinned, or if it is, to and from
         * pieces must be aligned with king
         *
         * @param move
         * @return true
         * @return false
         */
        bool is_legal(Move &move);

        /**
         * @brief Get the bitboard corresponding to the attackers
         *
         * @return Bitboard
         */
        Bitboard get_attack_mask();

        /**
         * @brief Get the checkmask to filter moves that will result in
         * check
         *
         * @return Bitboard
         */
        Bitboard get_checkmask();

        /**
         * @brief Register a move to the main move list, if it is legal
         *
         * @param move
         */
        void register_move(Move &move);

        /**
         * @brief Get the pieces attacking the king
         *
         * @param allies_include
         * @param allies_exclude
         * @param enemies_exclude
         * @return Bitboard
         */
        Bitboard get_attackers(Bitboard allies_include = 0,
                               Bitboard allies_exclude = 0,
                               Bitboard enemies_exclude = 0);

        /**
         * Generate all legal moves
         * If move list is empty, then player is in checkmate
         */

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
        void set_at(const Square sq, const Piece &piece);

        /**
         * @brief Get a piece on the board by coordinates
         *
         * @param row
         * @param col
         * @return Piece
         */
        Piece get_at_coords(int row, int col);

        /**
         * @brief Set a piece on the board by coordinates
         *
         * @param row
         * @param col
         * @param piece
         */
        void set_at_coords(int row, int col, const Piece &piece);

        /**
         * @brief Clear a square on the board
         *
         * @param sq
         */
        void clear_at(const Square sq);

        /**
         * @brief Perform a null move, skipping the current turn
         *
         */
        void skip_turn();

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
         * @brief Tests if the current player's king is in check
         *
         * @return true
         * @return false
         */
        bool is_check();

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
        bool is_draw();

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
         * @brief Get all legal moves available to the current player
         *
         */
        inline const std::vector<Move> &get_moves() {
            return _states[_current_state]._legal_moves;
        }

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
        inline Bitboard get_bitboard(Piece piece) {
            return _states[_current_state]._bitboards[piece.get_index()];
        }

        /**
         * @brief Get the bitboard associated with a piece
         *
         * @param type
         * @param color
         * @return Bitboard
         */
        inline Bitboard get_bitboard(PieceType type, Color color) {
            return _states[_current_state]
                ._bitboards[color * PieceType::NPieces + type];
        }

        /**
         * @brief Get the bitboard associated with a color
         *
         * @param color
         * @return Bitboard
         */
        inline Bitboard get_bitboard(Color color) {
            return _states[_current_state]
                ._bitboards[2 * PieceType::NPieces + color];
        }

        /**
         * @brief Get a bitboard by idex
         *
         * @param index
         * @return Bitboard
         */
        inline Bitboard get_bitboard(int index) {
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