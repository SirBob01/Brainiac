#pragma once

#include <chrono>
#include <string>
#include <vector>

namespace Brainiac {
    using namespace std::chrono_literals;

    /**
     * @brief Time measurement in seconds.
     *
     */
    using Seconds = std::chrono::duration<float>;

    /**
     * @brief Tokenize a string by splitting on a delimiter
     *
     * @param base
     * @param delimiter
     * @return std::vector<std::string>
     */
    std::vector<std::string> tokenize(std::string base, char delimiter = ' ');

    /**
     * @brief Get the current time since epoch.
     *
     * @return Seconds
     */
    Seconds time();
} // namespace Brainiac