#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>

class IOHandler {
public:
    static std::string ReadFile(const std::string& path); // Returns the data of a file as a string
    static std::vector<std::string> ReadFileLines(const std::string& path); // Returns each line of a file as a string
    static std::vector<std::string> GetFolders(const std::string& path); // Returns the name of every sub folder of a given path
    static std::string GetFilename(const std::string& path); // Returns the filename of a given file path with file extension
    static std::string GetFilenameRaw(const std::string& path); // Returns the filename of a given file path without file extension
    static std::string GetLastFolder(const std::string& path); // Returns the name of the last folder of a given file path
};
