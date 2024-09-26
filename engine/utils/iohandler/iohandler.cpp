#include "iohandler.h"

std::string IOHandler::ReadFile(std::string path) {
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

std::vector<std::string> IOHandler::GetFolders(const std::string& path) {
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