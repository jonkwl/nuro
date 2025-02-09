#pragma once

#include <string>
#include <vector>

namespace IOHandler
{

	// Returns the data of a file as a string
	std::string readFile(const std::string& path);

	// Returns each line of a file as a string
	std::vector<std::string> readFileLines(const std::string& path);

	// Returns the name of every sub folder of a given path
	std::vector<std::string> getFolders(const std::string& path);

	// Returns the filename of a given file path with file extension
	std::string getFilename(const std::string& path);

	// Returns the filename of a given file path without file extension
	std::string getFilenameRaw(const std::string& path);

	// Returns the extension of a given filename 
	std::string getFileExtension(const std::string& filename);
	
	// Returns all paths of all files within given path with valid extension
	std::vector<std::string> getFilesWithExtensions(const std::string& path, const std::vector<std::string>& extensions);

};
