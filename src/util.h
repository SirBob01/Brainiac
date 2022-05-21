#ifndef BRAINIAC_UTIL_H_
#define BRAINIAC_UTIL_H_

#include <string>
#include <vector>

namespace brainiac::util {
    /**
     * @brief Tokenize a string by splitting on a delimiter
     *
     * @param base String to be split
     * @param delimiter Delimiter character
     * @return std::vector<std::string> List of tokens
     */
    std::vector<std::string> tokenize(std::string base, char delimiter);
} // namespace brainiac::util

#endif