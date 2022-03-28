#ifndef CHESS_TRANSPOSITIONS_H_
#define CHESS_TRANSPOSITIONS_H_

#define TRANSPOSITION_TABLE_SIZE 1 << 16

#include <array>
#include <vector>

#include "board.h"

namespace chess {
    struct TableNode {
        uint64_t key;
        int depth;
        int value;
    };

    /**
     * A hash table of visited nodes
     */
    class Transpositions {
        std::array<std::vector<TableNode>, TRANSPOSITION_TABLE_SIZE> _table;

      public:
        /**
         * Set the value of a board position
         */
        void set(Board &board, int depth, int value);

        /**
         * Get the stored value of a position
         */
        int get(Board &board);

        /**
         * Check if board has been visited
         */
        bool contains(Board &board);

        /**
         * Print the transposition table for debugging
         */
        void print();
    };
} // namespace chess

#endif