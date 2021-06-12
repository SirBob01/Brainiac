#include "species.h"

namespace chess::neural {
    Specie::Specie(NEATParameters params) {
        _params = params;
        _stagnation_count = 0;
    }

    int Specie::get_size() {
        return _members.size();
    }

    void Specie::add(Genome genome) {
        _members.push_back(genome);
    }

    Genome Specie::get_repr() {
        return _members[0];
    }

    void Specie::repopulate() {

    }
}