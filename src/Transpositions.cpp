#include "Transpositions.hpp"

namespace Brainiac {
    Transpositions::Transpositions() : _table(TABLE_SIZE) {}

    TableEntry Transpositions::get(Position &pos) const {
        return _table[pos.hash() & TABLE_MASK];
    }

    void Transpositions::set(Position &pos, TableEntry entry) {
        // Overwrite if new entry's depth if higher
        unsigned index = pos.hash() & TABLE_MASK;
        if (_table[index].type == NodeType::Invalid ||
            _table[index].depth >= entry.depth) {
            _table[index] = entry;
        }
    }
} // namespace Brainiac