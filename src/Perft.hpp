#pragma once

#include <cstdint>
#include <functional>

#include "Game.hpp"

namespace Brainiac {
    /**
     * @brief Recursive perft function. Useful for debugging the move generator.
     *
     * @param game
     * @param depth
     * @param max_depth
     * @param cb
     * @return uint64_t
     */
    uint64_t perft(Game &game,
                   unsigned depth,
                   unsigned max_depth,
                   std::function<void(Move, uint64_t)> cb = nullptr);
} // namespace Brainiac