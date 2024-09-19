#include "UCI.hpp"
#include "Engine.hpp"

namespace Brainiac {
    UCI::UCI() {
        _debug = false;
        _running = true;

        // Assign search callbacks

        _search.set_iterative_callback([](IterativeInfo &info) {
            std::ostringstream stream;
            stream << "info depth " << static_cast<unsigned>(info.depth);
            stream << " currmove " << info.move.standard_notation();
            stream << " currmovenumber " << info.move_number;
            std::cout << stream.str() << std::endl;
        });

        _search.set_pv_callback([](PVInfo &info) {
            unsigned time_ms = info.time.count() * 1000;
            unsigned nps = info.nodes / info.time.count();

            std::ostringstream stream;
            stream << "info depth " << static_cast<unsigned>(info.depth);
            stream << " time " << time_ms;
            stream << " nodes " << info.nodes;
            stream << " nps " << nps;
            stream << " score cp " << info.value;
            stream << " pv";
            for (unsigned i = 0; i < info.pv_length; i++) {
                stream << " " << info.pv[i].standard_notation();
            }
            std::cout << stream.str() << std::endl;
        });

        _search.set_bestmove_callback([](Move &move) {
            std::ostringstream stream;
            stream << "bestmove " << move.standard_notation();
            std::cout << stream.str() << std::endl;
        });

        // Assign command handlers

        _command_map["uci"] = [&](Tokens &args) {
            std::cout << "id name Brainiac " << get_engine_version() << "\n";
            std::cout << "id author Keith Leonardo\n";
            std::cout << "uciok" << std::endl;
        };

        _command_map["isready"] = [&](Tokens &args) {
            std::cout << "readyok" << std::endl;
        };

        _command_map["quit"] = [&](Tokens &args) {
            _search.stop();
            _running = false;
        };

        _command_map["debug"] = [&](Tokens &args) {
            if (args.empty()) return;

            if (args[0] == "on") {
                _debug = true;
            } else if (args[0] == "off") {
                _debug = false;
            }
        };

        _command_map["position"] = [&](Tokens &args) {
            if (args.empty()) return;

            unsigned moves_offset = 1;
            if (args[0] == "startpos") {
                _position = Position(DEFAULT_BOARD_FEN, _hasher);
            } else if (args[0] == "fen" && args.size() >= 7) {
                std::string fen = "";
                for (unsigned i = 0; i < 6; i++) {
                    fen += args[i + 1];
                    if (i < 5) {
                        fen += " ";
                    }
                }
                _position = Position(fen, _hasher);
                moves_offset = 7;
            }

            // Play move sequence
            if (args[moves_offset] == "moves") {
                for (unsigned i = moves_offset + 1; i < args.size(); i++) {
                    Move move = _position.find_move(args[i]);
                    if (move.type() == MoveType::Skip) {
                        std::cout << args[i] << " is not a valid move."
                                  << std::endl;
                        break;
                    }
                    _position.make(move);
                }
            }
        };

        _command_map["go"] = [&](Tokens &args) {
            SearchLimits limits;
            unsigned perft = 0;
            for (unsigned i = 0; i < args.size(); i++) {
                std::string key = args[i];
                if (key == "perft") {
                    perft = stoi(args[++i]);
                } else if (key == "wtime") {
                    limits.white_time = Seconds(stoi(args[++i]) / 1000.0f);
                } else if (key == "btime") {
                    limits.black_time = Seconds(stoi(args[++i]) / 1000.0f);
                } else if (key == "winc") {
                    limits.white_increment = Seconds(stoi(args[++i]) / 1000.0f);
                } else if (key == "binc") {
                    limits.black_increment = Seconds(stoi(args[++i]) / 1000.0f);
                } else if (key == "movetime") {
                    limits.move_time = Seconds(stoi(args[++i]) / 1000.0f);
                } else if (key == "depth") {
                    limits.depth = stoi(args[++i]);
                } else if (key == "nodes") {
                    limits.nodes = stoi(args[++i]);
                } else if (key == "movestogo") {
                    limits.moves_to_go = stoi(args[++i]);
                }
            }

            if (perft) {
                perft_handler(perft);
            } else {
                if (_search_thread.joinable()) {
                    _search_thread.join();
                }
                _search_thread = std::thread(&Search::go,
                                             &_search,
                                             std::ref(_position),
                                             limits);
            }
        };

        _command_map["setoption"] = [&](Tokens &args) {
            // TODO: Add customizable options to Search
        };

        _command_map["stop"] = [&](Tokens &args) { _search.stop(); };

        _command_map["ucinewgame"] = [&](Tokens &args) { _search.reset(); };

        // Non-UCI commands (for debugging)
        _command_map["help"] = [&](Tokens &args) {
            std::cout << "Brainiac is a simple chess engine. It uses the "
                         "Universal Chess Interface (UCI) to communicate with "
                         "a GUI. Below are the available commands:\n";
            for (auto &entry : _command_map) {
                std::cout << " " << entry.first << "\n";
            }
            std::cout << std::endl;
        };

        _command_map["print"] = [&](Tokens &args) {
            _position.print();
            std::cout << "FEN: " << _position.fen() << "\n\n";
            std::cout << "Hash:\n";
            print_bitboard(_position.hash());
            std::cout << std::flush;
        };

        _command_map["eval"] = [&](Tokens &args) {
            Value material = compute_material(_position.board());
            Value placement = compute_placement(_position.board());
            Value eval = evaluate(_position);

            std::cout << "Material (White PoV): " << material << "\n";
            std::cout << "Placement (White PoV): " << placement << "\n";
            std::cout << "Evaluation: " << eval << std::endl;
        };
    }

    void UCI::perft_handler(unsigned depth) {
        auto cb = [&](Move move, uint64_t count) {
            std::cout << move.standard_notation() << ": " << count << "\n";
        };
        Seconds start = time();
        uint64_t nodes = perft(_position, depth, depth, cb);
        Seconds stop = time();
        Seconds duration = stop - start;

        std::cout << "Perft(" << depth << ") = " << nodes << " ("
                  << duration.count() << "s)" << std::endl;
    }

    void UCI::run() {
        std::string input;

        while (_running) {
            std::getline(std::cin, input);

            std::vector<std::string> tokens = tokenize(input);
            if (tokens.empty()) {
                continue;
            }

            // Run appropriate command from the map, if available
            if (_command_map.contains(tokens[0])) {
                CommandHandler &handler = _command_map.at(tokens[0]);
                tokens.erase(tokens.begin());

                // I don't like this but exceptions fuck up my terminal
                try {
                    handler(tokens);
                } catch (std::exception &_) {
                    _running = false;
                }
            } else {
                std::cout << "'" << tokens[0]
                          << "' is not a command. Type 'help' to see full list."
                          << std::endl;
            }
        }

        if (_search_thread.joinable()) {
            _search_thread.join();
        }
    }
} // namespace Brainiac