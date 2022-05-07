#ifndef BRAINIAC_UTIL_H_
#define BRAINIAC_UTIL_H_

#include <string>
#include <vector>

namespace brainiac::util {
    /**
     * Tokenize a string by splitting on a delimiter.
     */
    std::vector<std::string> tokenize(std::string base, char delimiter);
} // namespace brainiac::util

#endif