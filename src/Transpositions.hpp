#pragma once

#include <vector>

#include "Evaluation.hpp"
#include "Move.hpp"
#include "Numeric.hpp"
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
     * @brief Transposition node.
     *
     */
    struct Node {
        NodeType type = NodeType::Invalid;
        Depth depth;
        Value value;
        Move move;
        Hash hash;
    };

    /**
     * @brief Transposition table.
     *
     */
    class Transpositions {
        std::vector<Node> _table;

      public:
        Transpositions();

        /**
         * @brief Read an entry from the table.
         *
         * @param position
         * @return Node
         */
        Node get(Position &position) const;

        /**
         * @brief Set an entry to the table.
         *
         * @param position
         * @param type
         * @param depth
         * @param value
         * @param move
         */
        void set(Position &position,
                 NodeType type,
                 Depth depth,
                 Value value,
                 Move move);

        /**
         * @brief Clear the table.
         *
         */
        void clear();
    };
} // namespace Brainiac