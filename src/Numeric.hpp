#pragma once

#include <limits>

namespace Brainiac {
    /**
     * @brief Position value.
     *
     */
    using Value = short;

    /**
     * @brief Depth value.
     *
     */
    using Depth = unsigned short;

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
     * @brief Maximum possible depth of the search.
     *
     */
    constexpr Depth MAX_DEPTH = 7;
} // namespace Brainiac