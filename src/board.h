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
     * Linked list representing chronological board state
     * Allows forward and backward movement in time (undo/redo moves)
     * Stores information that can be difficult to undo when changed (e.g.,
     * castling rights)
     */
    struct BoardState {
        // Represent the positions of each of the 12 pieces on the board
        // Extra 2 bitboards represent white/black pieces in general
        uint64_t _bitboards[14] = {0};

        // qkQK (from most to least significant bit)
        uint8_t _castling_rights;
        Square _en_passant_target;

        // Unlike fullmoves, halfmoves depends on state of the board (pawn
        // advance or capture reset)
        int _halfmoves;

        // Bitboard representing the attackers on each square (excluding king)
        uint64_t _attackers = 0;
        std::vector<Move> _legal_moves;

        int _material = 0;
        uint64_t _hash;
    };

    /**
     * Represents a chess board's state
     */
    class Board {
        std::vector<BoardState> _states;
        int _current_state;

        Color _turn;
        int _fullmoves;

        /**
         * Generate all pseudo-legal moves for single step moves
         */
        void generate_step_moves(uint64_t bitboard,
                                 bool is_king,
                                 uint64_t (*mask_func)(uint64_t));

        /**
         * Slider moves need more information about the board
         */
        void generate_slider_moves(uint64_t bitboard,
                                   uint64_t (*mask_func)(uint64_t,
                                                         uint64_t,
                                                         uint64_t));

        /**
         * Pawn function has special cases (ugh.)
         */
        void generate_pawn_moves(uint64_t bitboard);

        /**
         * Create castling moves
         */
        void generate_castling_moves(uint64_t bitboard);

        /**
         * Test if a pseudo-legal move is legal
         * Algorithm for generating legal moves from pseudo legal?
         * - If king is the moving piece, make sure destination square is not an
         * attack target
         * - If move is an en passant, king must not currently be in check
         * - If non-king piece, it must not be pinned, or if it is, to and from
         * pieces must be aligned with king
         */
        bool is_legal(Move &move);

        /**
         * If a pseudo-legal move is legal, register it to the move list
         */
        void register_move(Move &move);

        /**
         * Get the pieces attacking the king
         */
        uint64_t get_attackers(uint64_t allies_include = 0,
                               uint64_t allies_exclude = 0,
                               uint64_t enemies_exclude = 0);

        /**
         * Generate all legal moves
         * If move list is empty, then player is in checkmate
         */
        void generate_moves();

        /**
         * Clone the current board state for the next turn
         */
        BoardState &push_state();

      public:
        Board(std::string fen_string =
                  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        Board(Board &other) : Board(other.generate_fen()){};

        /**
         * Generate a FEN string of the current state for serialization
         */
        std::string generate_fen();

        /**
         * Get the material score for the current board state
         * Positive values mean white has more material than black
         */
        int get_material();

        /**
         * Get the mobility score, which is the number of pseudo-legal moves
         * calculated for this turn
         */
        int get_mobility();

        /**
         * Get a piece on the board
         */
        Piece get_at(const Square &sq);

        /**
         * Set a piece on the board
         */
        void set_at(const Square &sq, const Piece &piece);

        /**
         * Get a piece on the board by coordinates
         */
        Piece get_at_coords(int row, int col);

        /**
         * Set a piece on the board by coordinates
         */
        void set_at_coords(int row, int col, const Piece &piece);

        /**
         * Clear a square on the board
         */
        void clear_at(const Square &sq);

        /**
         * Perform a null move, skipping the current turn
         */
        void skip_turn();

        /**
         * Execute a move and update internal state
         * Assumes move is legal
         */
        void execute_move(const Move &move);

        /**
         * Return board to the previous state
         * Only perform if board is not in the initial state
         */
        void undo_move();

        /**
         * Move board state forward in time
         * Only perform if board is not in the latest state
         */
        void redo_move();

        /**
         * Checks if the board is already at the initial state (cannot undo)
         */
        bool is_initial();

        /**
         * Checks if the board is already at the latest state (cannot redo)
         */
        bool is_latest();

        /**
         * Checks if the current turn's king is in check
         */
        bool is_check();

        /**
         * Checks if the current player is in checkmate
         */
        bool is_checkmate();

        /**
         * Checks if the board is in a stalemate
         */
        bool is_stalemate();

        /**
         * Checks if the game is a draw
         */
        bool is_draw();

        /**
         * Generate a valid chess move given shift positions
         * Used to validate move positions from user input
         */
        Move
        create_move(const Square &from, const Square &to, char promotion = 0);

        /**
         * Generate a valid chess move given a standard notation string
         */
        Move create_move(std::string standard_notation);

        /**
         * Get all legal moves available to the current player
         */
        const std::vector<Move> &get_moves();

        /**
         * Get the current number of halfmoves to enforce the 50-move rule
         */
        int get_halfmoves();

        /**
         * Return the bitfield corresponding to castling rights
         */
        uint8_t get_castling_rights();

        /**
         * Get the current turn (either White or Black)
         */
        Color get_turn();

        /**
         * Get the Zobrist hash of the board
         */
        uint64_t get_hash();

        /**
         * Print the board on the console
         */
        void print();
    };
} // namespace brainiac

#endif