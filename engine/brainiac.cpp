#include "brainiac.h"

namespace chess {
    Brainiac::Brainiac() {
        _params.phenome_params.hidden_activation = neural::lrelu;
        _params.phenome_params.output_activation = tanh;
        _params.population = 100;
        _params.target_species = 8;
    }

    void Brainiac::generate() {
        std::vector<neural::Point> inputs;
        std::vector<neural::Point> outputs = {{0, 0}};
        for(double i = 0; i < 8; i++) {
            for(double j = 0; j < 8; j++) {
                neural::Point node = {(i-3.5)/8, (j-3.5)/8};
                inputs.push_back(node);
            }
        }
        inputs.push_back({-1.0, 0});
        inputs.push_back({1.0, 0});
        _population0 = new neural::Brain(inputs, outputs, _params);
        _population1 = new neural::Brain(inputs, outputs, _params);
        _current_eval = 0;
    }

    Brainiac::~Brainiac() {
        delete _population0;
        delete _population1;
    }

    std::vector<double> Brainiac::vectorize(Board &board) {
        // Stores normalized input vector for neural network
        std::vector<double> vector;

        // All the pieces on the board
        for(int row = 0; row < 8; row++) {
            for(int col = 0; col < 8; col++) {
                Piece piece = board.get_at_coords(row, col);
                if(piece.is_empty()) {
                    vector.push_back(0);
                }
                else {
                    double i = 1+piece.get_piece_index();
                    vector.push_back(i / 12.0);
                }
            }
        }
        
        // Castling rights is (somewhat) independent of the current board states
        vector.push_back(board.get_castling_rights() / 16.0);

        // Enforcing the 50-move rule ensures finite length games
        vector.push_back(board.get_halfmoves() / 100.0);
        
        return vector;
    }

    void Brainiac::simulate(neural::Phenome &white, neural::Phenome &black, std::mutex &mutex) {
        Board board;
        while(true) {
            if(board.is_draw()) {
                // No change
                return;
            }
            else if(board.is_checkmate()) {
                std::lock_guard<std::mutex> lock(mutex);
                // Update fitness
                if(board.get_turn() == Color::Black) {
                    white.set_fitness(white.get_fitness() + 1);
                }
                else {
                    black.set_fitness(black.get_fitness() + 1);
                }
                return;
            }
            
            std::vector<Move> moves = board.get_moves();
            // Initially the worst possible score
            double best_score;
            double score;
            if(board.get_turn() == Color::White) {
                best_score = -1.0;
            }
            else {
                best_score = 1.0;
            }
            Move best_move = moves[0];

            for(auto &move : moves) {
                board.execute_move(move);
                if(board.get_turn() == Color::White) {
                    score = white.forward(vectorize(board))[0];
                    if(score >= best_score) {
                        best_score = score;
                        best_move = move;
                    }
                }
                else {
                    score = black.forward(vectorize(board))[0];
                    if(score <= best_score) {
                        best_score = score;
                        best_move = move;
                    }
                }
                board.undo_move();
            }
            board.execute_move(best_move);
        }
    }

    void Brainiac::train_host() {
        /**
         * Algorithm 1. Single game train:
         * 1. Generate opponent list from parasite population
         * 2. Evaluate the host population by simulating games against opponents
         * 3. Evolve the host
         * 4. Switch roles of populations
         */
        neural::Brain *host;
        neural::Brain *parasite;
        if(_current_eval == 0) {
            host = _population0;
            parasite = _population1;
        }
        else {
            host = _population1;
            parasite = _population0;
        }
        std::cout << "Generation " << host->get_generations() << " | ";
        auto &phenomes = host->get_phenomes();
        int n = phenomes.size();

        std::vector<neural::Phenome> opponents;
        for(auto &phenome : parasite->get_elites()) {
            opponents.push_back(phenome);
        }
        for(auto &phenome : parasite->get_hall_of_fame()) {
            opponents.push_back(phenome);
        }
        std::cout << opponents.size() << " opponents | ";
                
        // Reset fitness scores
        for(auto &phenome : phenomes) {
            phenome->set_fitness(0);
        }
        
        // Run simulation games
        std::vector<std::thread> threads;
        std::mutex mutex;
        
        for(auto &phenome : phenomes) {
            threads.push_back(std::thread([&phenome, &opponents, &mutex, n, this]() {
                for(auto &opponent : opponents) {
                    // Each phenome must play as both white and black
                    simulate(*phenome, opponent, mutex);
                    simulate(opponent, *phenome, mutex);
                }
            }));
        }
        for(auto &thread : threads) {
            thread.join();
        }
        std::cout << threads.size() << " threads executed | ";
        std::cout << "Host generation's best fitness: " << host->get_current_fittest().get_fitness() << "\n";
        std::cout << "Host global best fitness: " << host->get_fittest().get_fitness() << "\n";

        // Evolve
        host->evolve();

        // Switch population roles
        _current_eval = 1 - _current_eval;
    }

    void Brainiac::train() {
        train_host(); // Population 0
        train_host(); // Population 1
    }

    Move Brainiac::evaluate(Board &board) {
        std::vector<Move> moves = board.get_moves();
        // Lowest possible score is when you lose
        double best_score = -1.0;
        Move best_move;
        neural::Phenome best0 = _population0->get_fittest();
        neural::Phenome best1 = _population1->get_fittest();
        neural::Phenome &best = best0.get_fitness() > best1.get_fitness() ? best0 : best1;

        for(auto &move : moves) {
            board.execute_move(move);
            double score = best.forward(vectorize(board))[0];
            if(score >= best_score) {
                best_score = score;
                best_move = move;
            }
            board.undo_move();
        }
        return best_move;
    }
    
    bool Brainiac::load() {
        std::ifstream f0("p0.bin");
        std::ifstream f1("p1.bin");
        if(!f0.good() || !f1.good()) {
            return false;
        }
        _population0 = new neural::Brain("p0.bin", _params);
        _population1 = new neural::Brain("p1.bin", _params);
        return true;
    }

    void Brainiac::save() {
        _population0->save("p0.bin");
        _population1->save("p1.bin");
    }
}