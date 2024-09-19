#include "PVTable.hpp"

namespace Brainiac {
    PVTable::PVTable() : _table({}), _lengths({}) {}

    unsigned PVTable::get_offset(Depth ply) const { return MAX_DEPTH * ply; }

    unsigned PVTable::get_length(Depth ply) const { return _lengths[ply]; }

    Move PVTable::get(Depth ply, unsigned index) const {
        return _table[get_offset(ply) + index];
    }

    void PVTable::update(Depth ply, Move last) {
        unsigned offset = get_offset(ply);
        unsigned next_offset = get_offset(ply + 1);
        unsigned next_length = get_length(ply + 1);

        std::copy(_table.data() + next_offset,
                  _table.data() + next_offset + next_length,
                  _table.data() + offset + 1);

        _table[offset] = last;
        _lengths[ply] = next_length + 1;
    }

    void PVTable::clear(Depth ply) { _lengths[ply] = 0; }
} // namespace Brainiac