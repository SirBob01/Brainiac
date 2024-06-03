#include "MoveList.hpp"

namespace Brainiac {
    Move &MoveList::operator[](unsigned index) { return _list[index]; };

    const Move &MoveList::operator[](unsigned index) const {
        return _list[index];
    };

    unsigned MoveList::size() const { return _count; };

    void MoveList::clear() { _count = 0; };

    void MoveList::add(Square src, Square dst, MoveType type) {
        _list[_count++] = Move(src, dst, type);
    };

    void MoveList::add(Move move) { _list[_count++] = move; };

    const Move *MoveList::begin() const { return _list.data(); };

    const Move *MoveList::end() const { return _list.data() + _count; };
} // namespace Brainiac