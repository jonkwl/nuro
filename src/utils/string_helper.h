#pragma once

#include <string>
#include <vector>

class StringHelper
{
public:
    static bool startsWith(const std::string& str, const std::string& prefix);
    static std::vector<std::string> split(const std::string& str, const std::string& delimiter);
};

