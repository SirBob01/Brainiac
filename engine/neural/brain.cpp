#include "brain.h"

namespace chess::neural {
    Brain::Brain(std::vector<Point> inputs, std::vector<Point> outputs, NEATParameters params) {
        _inputs = inputs;
        _outputs = outputs;
        _params = params;

        _generations = 0;

        // Register a population of random genomes
        for(int i = 0; i < params.population; i++) {
            Genome *g = new Genome(_params.genome_params);
            add_genome(g);
        }
        generate_phenomes();
    }

    Brain::Brain(std::string filename) {
        std::ifstream infile;
        infile.open(filename, std::ios::binary | std::ios::in);
        infile.read(reinterpret_cast<char *>(&_params), sizeof(NEATParameters));
        infile.read(reinterpret_cast<char *>(&_generations), sizeof(int));

        int specie_count;
        infile.read(reinterpret_cast<char *>(&specie_count), sizeof(int));
        for(int i = 0; i < specie_count; i++) {
            int genome_count;
            infile.read(reinterpret_cast<char *>(&genome_count), sizeof(int));
            for(int j = 0; j < genome_count; j++) {
                std::unordered_map<Edge, EdgeGene, EdgeHash> edges;
                std::vector<NodeGene> nodes;

                // Save the edges
                int edge_count;
                infile.read(reinterpret_cast<char *>(&edge_count), sizeof(int));
                for(int k = 0; k < edge_count; k++) {
                    Edge edge;
                    EdgeGene gene;
                    infile.read(reinterpret_cast<char *>(&edge.from), sizeof(int));
                    infile.read(reinterpret_cast<char *>(&edge.to), sizeof(int));
                    infile.read(reinterpret_cast<char *>(&gene.weight), sizeof(double));
                    infile.read(reinterpret_cast<char *>(&gene.enabled), sizeof(bool));
                    edges[edge] = gene;
                }

                // Save the nodes
                int node_count;
                infile.read(reinterpret_cast<char *>(&node_count), sizeof(int));
                for(int k = 0; k < node_count; k++) {
                    NodeGene n;
                    infile.read(reinterpret_cast<char *>(&n.bias), sizeof(double));
                    infile.read(reinterpret_cast<char *>(&n.type), sizeof(NodeType));

                    // Read the function string
                    int len;
                    infile.read(reinterpret_cast<char *>(&len), sizeof(int));

                    char *tmp = new char[len+1];
                    tmp[len] = 0;
                    infile.read(tmp, sizeof(char) * len);
                    std::string function(tmp);
                    delete[] tmp;

                    // Set the remaining values
                    n.function = activations.at(function);
                    nodes.push_back(n);
                }
                add_genome(new Genome(nodes, edges, _params.genome_params));
            }
        }
        infile.close();
        generate_phenomes();
    }

    Brain::~Brain() {
        for(auto &specie : _species) {
            for(auto &genome : specie->get_members()) {
                delete genome;
            }
            delete specie;
        }
        for(auto &phenome : _phenomes) {
            delete phenome;
        }
    }

    double Brain::distance(Genome &a, Genome &b) {
        std::unordered_map<Edge, EdgeGene, EdgeHash> a_edges = a.get_edges();
        std::unordered_map<Edge, EdgeGene, EdgeHash> b_edges = b.get_edges();
        
        int N = std::max(a_edges.size(), b_edges.size());

        std::vector<Edge> disjoint;
        for(auto &e : b_edges) {
            if(a_edges.count(e.first) == 0) {
                disjoint.push_back(e.first);
            }
        }
        for(auto &e : a_edges) {
            if(b_edges.count(e.first) == 0) {
                disjoint.push_back(e.first);
            }
        }

        double t1 = static_cast<double>(disjoint.size()) / N;
        double t2 = 0;
        
        int n = 0;
        for(auto &e : a_edges) {
            if(b_edges.count(e.first)) {
                double w_diff = b_edges[e.first].weight - e.second.weight;
                t2 += std::fabs(w_diff);
                n++;
            }
        }
        if(n) {
            t2 /= n;
        }
        return t1 * _params.c1 + t2 * _params.c2;
    }

    Genome *Brain::crossover(Genome &a, Genome &b) {
        auto a_edges = a.get_edges();
        auto b_edges = b.get_edges();

        auto a_nodes = a.get_nodes();
        auto b_nodes = b.get_nodes();

        // Calculate disjoint and matching edges
        std::vector<Edge> matching;
        std::vector<Edge> disjoint;
        for(auto &e : b_edges) {
            if(a_edges.count(e.first) == 0) {
                disjoint.push_back(e.first);
            }
            else {
                matching.push_back(e.first);
            }
        }
        for(auto &e : a_edges) {
            if(b_edges.count(e.first) == 0) {
                disjoint.push_back(e.first);
            }
        }

        std::unordered_map<Edge, EdgeGene, EdgeHash> child_edges;
        std::vector<NodeGene> child_nodes;

        // Inherit matching genes from random parents
        for(auto &edge : matching) {
            if(random() < 0.5) {
                child_edges[edge] = a_edges[edge];
            }
            else {
                child_edges[edge] = b_edges[edge];
            }
        }

        // Inherit disjoint genes from fitter parent (or random if they're equally fit)
        for(auto &edge : disjoint) {
            if(a.get_fitness() > b.get_fitness()) {
                child_edges[edge] = a_edges[edge];
            }
            else if(a.get_fitness() < b.get_fitness()) {
                child_edges[edge] = b_edges[edge];
            }
            else {
                if(random() < 0.5) {
                    child_edges[edge] = a_edges[edge];
                }
                else {
                    child_edges[edge] = b_edges[edge];
                }
            }
        }

        // Inherit nodes from fitter parent (or random if they're equally fit)
        int max_node = 0;
        for(auto &e : child_edges) {
            max_node = std::max(max_node, e.first.from);
            max_node = std::max(max_node, e.first.to);
        }
        for(int n = 0; n <= max_node; n++) {
            if(a.get_fitness() > b.get_fitness()) {
                if(n < a_nodes.size()) {
                    child_nodes.push_back(a_nodes[n]);
                }
                else {
                    child_nodes.push_back(b_nodes[n]);
                }
            }
            else if(a.get_fitness() < b.get_fitness()) {
                if(n < b_nodes.size()) {
                    child_nodes.push_back(b_nodes[n]);
                }
                else {
                    child_nodes.push_back(a_nodes[n]);
                }
            }
            else {
                if(random() < 0.5) {
                    if(n < a_nodes.size()) {
                        child_nodes.push_back(a_nodes[n]);
                    }
                    else {
                        child_nodes.push_back(b_nodes[n]);
                    }
                }
                else {
                    if(n < b_nodes.size()) {
                        child_nodes.push_back(b_nodes[n]);
                    }
                    else {
                        child_nodes.push_back(a_nodes[n]);
                    }
                }
            }
        }

        return new Genome(child_nodes, child_edges, _params.genome_params);
    }
    
    void Brain::add_genome(Genome *genome) {
        // Test if the new genome fits in an existing specie
        for(auto &specie : _species) {
            Genome &repr = specie->get_repr();
            if(distance(repr, *genome) < _params.distance_threshold) {
                specie->add(genome);
                return;
            }
        }

        // Create a new species
        _species.push_back(new Specie(genome, _params));
    }

    void Brain::generate_phenomes() {
        _phenomes.clear();
        for(auto &s : _species) {
            for(auto &genome : s->get_members()) {
                _phenomes.push_back(new Phenome(*genome, _inputs, _outputs, _params.phenome_params));
            }            
        }
    }

    std::vector<Phenome *> &Brain::get_phenomes() {
        return _phenomes;
    }

    void Brain::evolve() {
        // Cull
        std::vector<Genome *> survivors;
        for(auto &phenome : _phenomes) {
            delete phenome;
        }
        for(auto &specie : _species) {
            specie->cull();
            for(auto &genome : specie->get_members()) {
                survivors.push_back(genome);
            }
            delete specie;
        }
        _species.clear();

        // Repopulate
        while(survivors.size() < _params.population) {
            double r = random();
            double cum_prob = 0;
            Genome *child = nullptr;
            if(r < _params.crossover_probability) {
                Genome *mom = survivors[randrange(0, survivors.size())]; 
                Genome *dad = survivors[randrange(0, survivors.size())];
                if(mom == dad) {
                    child = new Genome(*mom);
                    child->mutate();
                }
                else {
                    child = crossover(*mom, *dad);
                }
            }
            cum_prob += _params.crossover_probability;

            if(r >= cum_prob && r < cum_prob + _params.mutation_probability) {
                Genome *parent = survivors[randrange(0, survivors.size())]; 
                child = new Genome(*parent);
                child->mutate();
            }
            cum_prob += _params.mutation_probability;

            if(r >= cum_prob && r < cum_prob + _params.clone_probability) {
                Genome *parent = survivors[randrange(0, survivors.size())]; 
                child = new Genome(*parent);
            }
            cum_prob += _params.clone_probability;
            assert(cum_prob == 1.0);
            survivors.push_back(child);
        }

        // Speciate
        for(int i = 0; i < survivors.size(); i++) {
            Genome *g = survivors[i];
            add_genome(g);
        }
        generate_phenomes();
        _generations++;
    }

    int Brain::get_generations() {
        return _generations;
    }

    Phenome &Brain::get_fittest() {
        Phenome &current_best = *_phenomes[0];
        for(auto &phenome : _phenomes) {
            if(phenome->get_fitness() > current_best.get_fitness()) {
                current_best = *phenome;
            }
        }
        return current_best;
    }

    void Brain::save(std::string filename) {
        std::ofstream outfile;
        outfile.open(filename, std::ios::binary | std::ios::out);
        outfile.write(reinterpret_cast<char *>(&_params), sizeof(NEATParameters));
        outfile.write(reinterpret_cast<char *>(&_generations), sizeof(int));

        int specie_count = _species.size();
        outfile.write(reinterpret_cast<char *>(&specie_count), sizeof(int));
        for(auto &specie : _species) {
            int genome_count = specie->get_members().size();
            outfile.write(reinterpret_cast<char *>(&genome_count), sizeof(int));
            for(auto &genome : specie->get_members()) {
                auto edges = genome->get_edges();
                auto nodes = genome->get_nodes();

                // Save the edges
                int edge_count = edges.size();
                outfile.write(reinterpret_cast<char *>(&edge_count), sizeof(int));
                for(auto &e : edges) {
                    Edge edge = e.first;
                    EdgeGene gene = e.second;
                    outfile.write(reinterpret_cast<char *>(&edge.from), sizeof(int));
                    outfile.write(reinterpret_cast<char *>(&edge.to), sizeof(int));
                    outfile.write(reinterpret_cast<char *>(&gene.weight), sizeof(double));
                    outfile.write(reinterpret_cast<char *>(&gene.enabled), sizeof(bool));
                }

                // Save the nodes
                int node_count = nodes.size();
                outfile.write(reinterpret_cast<char *>(&node_count), sizeof(int));
                for(auto &n : nodes) {
                    std::string function;
                    for(auto &a : activations) {
                        if(a.second == n.function) {
                            function = a.first;
                            break;
                        }
                    }
                    outfile.write(reinterpret_cast<char *>(&n.bias), sizeof(double));
                    outfile.write(reinterpret_cast<char *>(&n.type), sizeof(NodeType));

                    // Save function string
                    int len = function.length();
                    outfile.write(reinterpret_cast<char *>(&len), sizeof(int));
                    outfile.write(function.c_str(), sizeof(char) * len);
                }
            }
        }
        outfile.close();
    }
}