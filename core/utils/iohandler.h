#pragma once

#include <string>
#include <vector>

namespace IOHandler
{

	std::string readFile(const std::string& path);                   // Returns the data of a file as a string
	std::vector<std::string> readFileLines(const std::string& path); // Returns each line of a file as a string
	std::vector<std::string> getFolders(const std::string& path);    // Returns the name of every sub folder of a given path
	std::string getFilename(const std::string& path);                // Returns the filename of a given file path with file extension
	std::string getFilenameRaw(const std::string& path);             // Returns the filename of a given file path without file extension
	std::string getLastFolder(const std::string& path);              // Returns the name of the last folder of a given file path

};
