#include "species.h"

namespace chess::neural {
    Specie::Specie(Genome *representative, NEATParameters params) {
        _members.push_back(representative);
        _params = params;
        _stagnation_count = 0;
    }

    void Specie::add(Genome *genome) {
        _members.push_back(genome);
    }

    Genome &Specie::get_repr() {
        return *_members[0];
    }

    std::vector<Genome *> &Specie::get_members() {
        return _members;
    }

    void Specie::cull() {
        // Sort and destroy worst performers among species
        std::sort(_members.begin(), _members.end(), [](Genome *a, Genome *b) {
            return a->get_fitness() > b->get_fitness();
        });
        int index = 1 + ((1.0 - _params.cull_percent) * _members.size());
        for(int i = index; i < _members.size(); i++) {
            delete _members[i];
        }
        while(_members.size() > index) {
            _members.pop_back();
        }
    }

    void Specie::adjust_fitness() {
        int n = _members.size();
        for(auto &genome : _members) {
            double fitness = genome->get_fitness();
            genome->set_fitness(fitness / _members.size());
        }
    }
}