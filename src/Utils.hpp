#pragma once

#include <string>
#include <vector>

namespace Brainiac {
    /**
     * @brief Tokenize a string by splitting on a delimiter
     *
     * @param base
     * @param delimiter
     * @return std::vector<std::string>
     */
    std::vector<std::string> tokenize(std::string base, char delimiter = ' ');
} // namespace Brainiac