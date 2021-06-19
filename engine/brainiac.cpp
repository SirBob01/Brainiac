#include "brainiac.h"

namespace chess {
    Brainiac::Brainiac() {
        _params.phenome_params.hidden_activation = neural::lrelu;
        _params.phenome_params.output_activation = tanh;
        _params.population = 25;
        _params.target_species = 3;

        _savefile = "brainiac.bin";
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
        inputs.push_back({-0.6, 0});
        inputs.push_back({0.6, 0});
        _brain = std::make_unique<neural::Brain>(inputs, outputs, _params);
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

    void Brainiac::simulate(neural::Phenome &white, neural::Phenome &black) {
        Board board;
        while(true) {
            if(board.is_draw()) {
                // No change
                return;
            }
            else if(board.is_checkmate()) {
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

    void Brainiac::train() {
        /**
         * Algorithm 1. Single game train:
         * 1. Generate all possible pairs of phenomes
         * 2. For each pair, run a game and update the fitness scores
         * 3. Evolve
         */
        std::cout << "Generation " << _brain->get_generations() << " | ";
        auto &phenomes = _brain->get_phenomes();
        int n = phenomes.size();
        std::vector<std::thread> threads;
        for(int i = 0; i < n-1; i++) {
            for(int j = i+1; j < n; j++) {
                threads.push_back(std::thread([phenomes, i, j, this]() {
                    // Each phenome must play as both white and black
                    simulate(*phenomes[i], *phenomes[j]);
                    simulate(*phenomes[j], *phenomes[i]);
                }));
            }
        }
        for(auto &thread : threads) {
            thread.join();
        }
        std::cout << "Best fitness: " << _brain->get_fittest().get_fitness() << "\n";
        _brain->evolve();
    }

    Move Brainiac::evaluate(Board &board) {
        std::vector<Move> moves = board.get_moves();
        // Lowest possible score is when you lose
        double best_score = -1.0;
        Move best_move;

        for(auto &move : moves) {
            board.execute_move(move);
            neural::Phenome best = _brain->get_fittest();
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
        std::ifstream f(_savefile);
        if(!f.good()) {
            return false;
        }
        _brain = std::make_unique<neural::Brain>(_savefile);
        return true;
    }

    void Brainiac::save() {
        _brain->save(_savefile);
    }
}