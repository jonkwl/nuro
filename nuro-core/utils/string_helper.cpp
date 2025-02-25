#include "string_helper.h"

namespace StringHelper
{

	bool startsWith(const std::string& str, const std::string& prefix)
	{
		// Check if string is shorter than prefix
		if (str.length() < prefix.length())
		{
			return false;
		}
		// Compare beginning of string with prefix
		return str.compare(0, prefix.length(), prefix) == 0;
	}

	std::vector<std::string> split(const std::string& str, const std::string& delimiter)
	{
		std::vector<std::string> result;
		size_t start = 0;
		size_t end = str.find(delimiter);

		// Loop until no more delimiters are found
		while (end != std::string::npos)
		{
			// Add the substring before the delimiter to the result
			result.push_back(str.substr(start, end - start));
			// Move past the delimiter
			start = end + delimiter.length();
			// Find the next delimiter
			end = str.find(delimiter, start);
		}
		// Add the last segment after the final delimiter
		result.push_back(str.substr(start));

		return result;
	}

}
