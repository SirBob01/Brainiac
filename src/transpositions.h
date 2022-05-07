#ifndef BRAINIAC_TRANSPOSITIONS_H_
#define BRAINIAC_TRANSPOSITIONS_H_

#define TRANSPOSITION_TABLE_SIZE 0x1000000
#define TRANSPOSITION_HASH_MASK  0xFFFFFF

#include <vector>

#include "board.h"
#include "move.h"

namespace brainiac {
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
        Move best_move;
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
} // namespace brainiac

#endif