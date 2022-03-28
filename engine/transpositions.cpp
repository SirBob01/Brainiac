#include "transpositions.h"

namespace chess {
    void Transpositions::set(Board &board, int depth, int value) {
        uint64_t key = board.get_hash();
        int index = key & (_table.size() - 1);

        auto &bucket = _table[index];
        if (!contains(board)) {
            bucket.push_back({key, depth, value});
        }
    }

    int Transpositions::get(Board &board) {
        uint64_t key = board.get_hash();
        auto &bucket = _table[key & (_table.size() - 1)];
        for (auto &node : bucket) {
            if (node.key == key)
                return node.value;
        }
        return 0;
    }

    bool Transpositions::contains(Board &board) {
        uint64_t key = board.get_hash();
        auto &bucket = _table[key & (_table.size() - 1)];
        for (auto &node : bucket) {
            if (node.key == key)
                return true;
        }
        return false;
    }

    void Transpositions::print() {
        int collisions = 0;
        int total = 0;
        for (auto &bucket : _table) {
            if (bucket.empty())
                continue;
            std::cout << " - ";
            for (auto &node : bucket) {
                total++;
                std::cout << "* ";
            }
            collisions += bucket.size() - 1;
            std::cout << "\n";
        }
        std::cout << collisions << " collision(s) | " << total << " total.\n";
    }
} // namespace chess