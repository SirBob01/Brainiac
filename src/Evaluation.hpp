#pragma once

#include <array>

#include "Position.hpp"
#include "Transpositions.hpp"

namespace Brainiac {
    /**
     * @brief Piece weights from white's perspective.
     *
     */
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
     * @brief Compute the material score from white's perspective.
     *
     * @param board
     * @return short
     */
    short compute_material(const Board &board);

    /**
     * @brief Evaluate a position for the current turn.
     *
     * @param pos
     * @return short
     */
    short evaluate(Position &pos);
} // namespace Brainiac