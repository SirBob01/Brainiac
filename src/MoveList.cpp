#include "MoveList.hpp"

namespace Brainiac {
    Move MoveList::operator[](unsigned index) const { return _list[index]; };

    unsigned MoveList::size() const { return _count; };

    void MoveList::clear() { _count = 0; };

    void MoveList::add(Square src, Square dst, MoveType type) {
        _list[_count++] = Move(src, dst, type);
    };

    Move *MoveList::begin() { return _list.data(); };

    Move *MoveList::end() { return _list.data() + _count; };
} // namespace Brainiac