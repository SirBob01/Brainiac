#pragma once

#include <array>

#include "Position.hpp"

namespace Brainiac {
    constexpr std::array<short, 12> PIECE_WEIGHTS = {
        40,
        10,
        55,
        30,
        33,
        95,
        -40,
        -10,
        -55,
        -30,
        -33,
        -95,
    };

    /**
     * @brief Compute the material score of the current position.
     *
     * @param pos
     * @return short
     */
    short compute_material(Position &pos);
} // namespace Brainiac