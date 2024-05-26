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
         * @param pos
         * @return Node
         */
        Node get(Position &pos) const;

        /**
         * @brief Set an entry to the table.
         *
         * @param pos
         * @param node
         */
        void set(Position &pos, Node node);
    };
} // namespace Brainiac