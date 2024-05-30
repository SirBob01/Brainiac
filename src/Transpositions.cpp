#include "Transpositions.hpp"

namespace Brainiac {
    Transpositions::Transpositions() : _table(TABLE_SIZE) { clear(); }

    Node Transpositions::get(Position &pos) const {
        return _table[pos.hash() & TABLE_MASK];
    }

    void Transpositions::set(Position &pos, Node node) {
        // Overwrite if new entry's depth if higher
        unsigned index = pos.hash() & TABLE_MASK;
        if (_table[index].type == NodeType::Invalid ||
            _table[index].depth >= node.depth) {
            _table[index] = node;
        }
    }

    void Transpositions::clear() {
        Node node;
        node.type = NodeType::Invalid;
        std::fill(_table.begin(), _table.end(), node);
    }
} // namespace Brainiac