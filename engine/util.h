#ifndef CHESS_UTIL_H_
#define CHESS_UTIL_H_

#include <string>
#include <vector>

namespace chess::util {
    /**
     * Tokenize a string by splitting on a delimiter.
     */
    std::vector<std::string> tokenize(std::string base, char delimiter);
} // namespace chess::util

#endif