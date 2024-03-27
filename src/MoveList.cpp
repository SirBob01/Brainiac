#include "MoveList.hpp"

namespace Brainiac {
    Move MoveList::operator[](unsigned index) const { return _list[index]; };

    unsigned MoveList::size() const { return _count; };

    void MoveList::clear() { _count = 0; };

    void MoveList::add(Square from, Square to, MoveType type) {
        _list[_count++] = Move(from, to, type);
    };

    Move *MoveList::begin() { return _list.data(); };

    Move *MoveList::end() { return _list.data() + _count; };
} // namespace Brainiac