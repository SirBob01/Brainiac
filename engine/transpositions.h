#ifndef CHESS_TRANSPOSITIONS_H_
#define CHESS_TRANSPOSITIONS_H_

#define TRANSPOSITION_TABLE_SIZE 1 << 16

#include <array>

#include "board.h"

namespace chess {
    struct TableNode {
        uint64_t key;
        int depth;
        int value;

        TableNode *next = nullptr;

        TableNode(uint64_t key, int depth, int value) 
        : key(key), depth(depth), value(value) {};
        ~TableNode();
    };

    class Transpositions {
        std::array<TableNode *, TRANSPOSITION_TABLE_SIZE> _table;
    
    public:
        Transpositions();
        ~Transpositions();

        /**
         * Set the value of a board position
         */
        void set(Board &board, int depth, int value);

        /**
         * Get the stored value of a position
         */
        int get(Board &board, int depth);

        /**
         * Check if board has been visited
         */
        bool contains(Board &board, int depth);
    };
}

#endif