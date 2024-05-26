#pragma once

#include <limits>

namespace Brainiac {
    /**
     * @brief Position value.
     *
     */
    using Value = short;

    /**
     * @brief Move value.
     *
     */
    using MoveValue = int;

    /**
     * @brief Depth value.
     *
     */
    using Depth = char;

    /**
     * @brief Maximum possible value of a position.
     *
     */
    constexpr Value MAX_VALUE = std::numeric_limits<Value>::max();

    /**
     * @brief Minimum possible value of a position.
     *
     */
    constexpr Value MIN_VALUE = -MAX_VALUE;

    /**
     * @brief Winning value (checkmate).
     *
     */
    constexpr Value WIN_VALUE = MAX_VALUE * 0.75;

    /**
     * @brief Maximum possible value of a move.
     *
     */
    constexpr MoveValue MAX_MOVE_VALUE = std::numeric_limits<MoveValue>::max();

    /**
     * @brief Minimum possible value of a move.
     *
     */
    constexpr MoveValue MIN_MOVE_VALUE = -MAX_MOVE_VALUE;

    /**
     * @brief Maximum possible depth of the search.
     *
     */
    constexpr Depth MAX_DEPTH = 8;
} // namespace Brainiac