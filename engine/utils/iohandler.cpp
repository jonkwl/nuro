#include "iohandler.h"

std::string IOHandler::ReadFile(const std::string& path) {
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

std::vector<std::string> IOHandler::ReadFileLines(const std::string& path) {
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

std::string IOHandler::GetFilename(const std::string& path)
{
    size_t pos = path.find_last_of("/\\");
    return (pos != std::string::npos) ? path.substr(pos + 1) : path;
}
