#pragma once

#include <iostream>
#include <unordered_map>

#include "Perft.hpp"
#include "Search.hpp"
#include "Utils.hpp"

namespace Brainiac {
    /**
     * @brief UCI subsystem.
     *
     */
    class UCI {
        using Tokens = std::vector<std::string>;
        using CommandHandler = std::function<void(Tokens &)>;
        using CommandMap = std::unordered_map<std::string, CommandHandler>;

        Hasher _hasher;
        Search _search;
        Position _position;

        CommandMap _command_map;
        CommandMap _subcommand_map;

        bool _debug;
        bool _running;

      public:
        UCI();

        /**
         * @brief Main loop.
         *
         */
        void run();
    };
} // namespace Brainiac