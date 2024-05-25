#pragma once

#include <vector>

#include "Position.hpp"

namespace Brainiac {
    /**
     * @brief Size of the transposition table.
     *
     */
    constexpr unsigned TABLE_SIZE = 1 << 24;

    /**
     * @brief Mask to compute the table index.
     *
     */
    constexpr unsigned TABLE_MASK = TABLE_SIZE - 1;

    /**
     * @brief Types of nodes depending on their value
     *
     */
    enum NodeType : uint8_t { Exact, Lower, Upper, Invalid };

    /**
     * @brief Lower score bound.
     *
     */
    constexpr int LOWER_BOUND = -1000000000;

    /**
     * @brief Upper score bound.
     *
     */
    constexpr int UPPER_BOUND = -LOWER_BOUND;

    /**
     * @brief Transposition entry.
     *
     */
    struct TableEntry {
        NodeType type = NodeType::Invalid;
        unsigned depth;
        int value;
    };

    /**
     * @brief Transposition table.
     *
     */
    class Transpositions {
        std::vector<TableEntry> _table;

      public:
        Transpositions();

        /**
         * @brief Read an entry from the table.
         *
         * @param pos
         * @return TableEntry
         */
        TableEntry get(Position &pos) const;

        /**
         * @brief Set an entry to the table.
         *
         * @param pos
         * @param entry
         */
        void set(Position &pos, TableEntry entry);
    };
} // namespace Brainiac