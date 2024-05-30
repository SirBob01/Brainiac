#include "UCI.hpp"
#include "Brainiac.hpp"

namespace Brainiac {
    UCI::UCI() {
        _debug = false;
        _running = true;

        _command_map["uci"] = [&](Tokens &args) {
            std::cout << "id name Brainiac " << get_engine_version() << "\n";
            std::cout << "id author Keith Leonardo\n";
            std::cout << "uciok\n";
        };

        _command_map["isready"] = [&](Tokens &args) {
            std::cout << "readyok\n";
        };

        _command_map["quit"] = [&](Tokens &args) { _running = false; };

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
                        std::cout << args[i] << " is not a valid move.\n";
                        break;
                    }
                    _position.make(move);
                }
            }
        };

        _command_map["go"] = [&](Tokens &args) {
            if (args.empty()) return;

            std::string subcommand = args[0];
            args.erase(args.begin());

            // Execute subcommands
            if (_subcommand_map.contains(subcommand)) {
                CommandHandler &handler = _subcommand_map.at(subcommand);
                handler(args);
            }
        };

        _command_map["setoption"] = [&](Tokens &args) {
            // TODO: Add customizable options to Search
        };

        _command_map["ucinewgame"] = [&](Tokens &args) { _search.reset(); };

        // Non-UCI commands (for debugging)
        _command_map["help"] = [&](Tokens &args) {
            std::cout << "Brainiac is a simple chess engine. It uses the "
                         "Universal Chess Interface (UCI) to communicate with "
                         "a GUI. Below are the available commands:\n";
            for (auto &entry : _subcommand_map) {
                std::cout << " go " << entry.first << "\n";
            }
            for (auto &entry : _command_map) {
                if (entry.first == "go") continue;
                std::cout << " " << entry.first << "\n";
            }
            std::cout << "\n";
        };

        _command_map["print"] = [&](Tokens &args) {
            _position.print();
            std::cout << "FEN: " << _position.fen() << "\n\n";
            std::cout << "Hash:\n";
            print_bitboard(_position.hash());
        };

        _command_map["eval"] = [&](Tokens &args) {
            Value material = compute_material(_position.board());
            Value placement = compute_placement(_position.board());
            Value eval = evaluate(_position);

            std::cout << "Material (White PoV): " << material << "\n";
            std::cout << "Placement (White PoV): " << placement << "\n";
            std::cout << "Evaluation: " << eval << "\n";
        };

        // Sub-commands for 'go'
        _subcommand_map["perft"] = [&](Tokens &args) {
            if (args.empty()) return;

            unsigned depth = std::stoi(args[0]);
            auto cb = [&](Move move, uint64_t count) {
                std::cout << move.standard_notation() << ": " << count << "\n";
            };
            Seconds start = time();
            uint64_t nodes = perft(_position, depth, depth, cb);
            Seconds stop = time();
            Seconds duration = stop - start;

            std::cout << "\n";
            std::cout << "Perft(" << depth << ") = " << nodes << " ("
                      << duration.count() << " s)\n";
        };
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
                std::cout
                    << "'" << tokens[0]
                    << "' is not a command. Type 'help' to see full list.\n";
            }
        }
    }
} // namespace Brainiac