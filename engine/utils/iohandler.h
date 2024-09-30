#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>

class IOHandler {
public:
    static std::string ReadFile(const std::string& path);
    static std::vector<std::string> ReadFileLines(const std::string& path);
    static std::vector<std::string> GetFolders(const std::string& path);
    static std::string GetFilename(const std::string& path);
};
