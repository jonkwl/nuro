#include "iohandler.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

namespace IOHandler {

    std::string readFile(const std::string& path) {
        std::ifstream fileStream(path);
        if (!fileStream.is_open()) {
            std::cerr << "File Reader | Failed to open file: " << path << std::endl;
            return "";
        }
        std::stringstream buffer;
        buffer << fileStream.rdbuf();
        fileStream.close();
        return buffer.str();
    }

    std::vector<std::string> readFileLines(const std::string& path) {
        std::ifstream fileStream(path);
        std::vector<std::string> lines;
        if (!fileStream.is_open()) {
            std::cerr << "File Reader | Failed to open file: " << path << std::endl;
            return lines;
        }

        std::string line;
        while (std::getline(fileStream, line)) {
            lines.push_back(line);
        }
        fileStream.close();
        return lines;
    }

    std::vector<std::string> getFolders(const std::string& path) {
        std::vector<std::string> folders;
        try {
            for (const auto& entry : std::filesystem::directory_iterator(path)) {
                if (entry.is_directory()) {
                    folders.push_back(entry.path().filename().string());
                }
            }
        }
        catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "File Reader | Error accessing path: " << e.what() << std::endl;
        }
        return folders;
    }

    std::string getFilename(const std::string& path)
    {
        size_t pos = path.find_last_of("/\\");
        return (pos != std::string::npos) ? path.substr(pos + 1) : path;
    }

    std::string getFilenameRaw(const std::string& path) {
        size_t pos = path.find_last_of("/\\");
        std::string filename = (pos != std::string::npos) ? path.substr(pos + 1) : path;
        size_t dotPos = filename.find_last_of('.');
        return (dotPos != std::string::npos) ? filename.substr(0, dotPos) : filename;
    }

    std::string getLastFolder(const std::string& path) {
        std::filesystem::path fsPath(path);
        if (fsPath.has_parent_path()) {
            return fsPath.parent_path().filename().string();
        }
        return "";
    }

}