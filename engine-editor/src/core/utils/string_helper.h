#pragma once

#include <string>
#include <vector>

namespace StringHelper
{
	bool startsWith(const std::string& str, const std::string& prefix);
	std::vector<std::string> split(const std::string& str, const std::string& delimiter);
};
