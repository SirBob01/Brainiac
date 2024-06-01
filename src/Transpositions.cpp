#include "Transpositions.hpp"

namespace Brainiac {
    Transpositions::Transpositions() : _table(TABLE_SIZE) { clear(); }

    Node Transpositions::get(Position &position) const {
        return _table[position.hash() & TABLE_MASK];
    }

    void Transpositions::set(Position &position,
                             NodeType type,
                             Depth depth,
                             Value value,
                             Move move) {
        // Overwrite if new entry's depth if higher
        Hash hash = position.hash();
        Node &node = _table[hash & TABLE_MASK];
        if (node.type == NodeType::Invalid ||
            (node.hash == hash && node.depth >= depth)) {
            node.type = type;
            node.depth = depth;
            node.value = value;
            node.move = move;
            node.hash = hash;
        }
    }

    void Transpositions::clear() {
        Node node;
        node.type = NodeType::Invalid;
        std::fill(_table.begin(), _table.end(), node);
    }
} // namespace Brainiac