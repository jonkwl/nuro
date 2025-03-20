#pragma once

#include <string>
#include <vector>

namespace StringHelper
{

	// Checks if a string starts with a prefix
	bool startsWith(const std::string& str, const std::string& prefix);

	// Splits a string by a delimiter
	std::vector<std::string> split(const std::string& str, const std::string& delimiter);

};
