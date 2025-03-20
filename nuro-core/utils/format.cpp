#include "format.h"

#include <cctype>
#include <algorithm>

namespace Format {

    std::string snakeCase(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
            return std::isspace(c) ? '_' : std::tolower(c);
            });
        return str;
    }

}