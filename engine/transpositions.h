#ifndef CHESS_TRANSPOSITIONS_H_
#define CHESS_TRANSPOSITIONS_H_

#define TRANSPOSITION_TABLE_SIZE 0x100000
#define TRANSPOSITION_HASH_MASK  0xFFFFF

#include <vector>

#include "board.h"

namespace chess {
    /**
     * Types of nodes depending on their value
     */
    enum NodeType { Exact = 0, Lower = 1, Upper = 2 };

    /**
     * An entry in the table
     */
    struct TableNode {
        uint64_t key;
        int depth;
        float value;
        NodeType type;
    };

    /**
     * A hash table of visited nodes
     */
    class Transpositions {
        std::vector<TableNode> _table;

      public:
        Transpositions() : _table(TRANSPOSITION_TABLE_SIZE){};

        /**
         * Set the entry of a board position
         */
        inline void set(Board &board, TableNode &node) {
            _table[board.get_hash() & TRANSPOSITION_HASH_MASK] = node;
        }

        /**
         * Get the entry of a board position
         */
        inline TableNode &get(Board &board) {
            return _table[board.get_hash() & TRANSPOSITION_HASH_MASK];
        }
    };
} // namespace chess

#endif