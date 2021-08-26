#include "transpositions.h"

namespace chess {
    TableNode::~TableNode() {
        if(next) {
            delete next;
        }
    }

    Transpositions::Transpositions() {
        _table.fill(nullptr);    
    }

    Transpositions::~Transpositions() {
        for(auto &bucket : _table) {
            if(bucket) {
                delete bucket;
            }
        }
    }

    void Transpositions::set(Board &board, int depth, int value) {
        uint64_t key = board.get_hash();
        int index = key % _table.size();
        TableNode *node = _table[index];
        if(!node) {
            _table[index] = new TableNode(key, depth, value);
        }
        else {
            while(node->next) {
                if(node->key == key && node->depth == depth) return;
                node = node->next;
            }
            if(node->key == key && node->depth == depth) return;
            node->next = new TableNode(key, depth, value);
        }
    }

    int Transpositions::get(Board &board, int depth) {
        uint64_t key = board.get_hash();
        TableNode *node = _table[key % _table.size()];
        while(node->key != key && node->depth != depth) {
            node = node->next;
        }
        return node->value;
    }

    bool Transpositions::contains(Board &board, int depth) {
        uint64_t key = board.get_hash();
        TableNode *node = _table[key % _table.size()];
        while(node) {
            if(node->key == key && node->depth == depth) {
                return true;
            }
            node = node->next;
        }
        return false;
    }
}