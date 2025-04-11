#include "fsutil.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <utils/console.h>

namespace fs = std::filesystem;

namespace IOUtils
{
    std::string readFile(const std::string& path)
    {
        std::error_code ec;
        if (!fs::exists(path, ec) || ec)
        {
            Console::out::warning("Filesystem", "Failed to find file: " + path);
            return "";
        }

        std::ifstream fileStream(path, std::ios::in);
        if (!fileStream.is_open())
        {
            Console::out::warning("Filesystem", "Failed to open file: " + path);
            return "";
        }

        std::stringstream buffer;
        buffer << fileStream.rdbuf();
        fileStream.close();
        return buffer.str();
    }

    std::vector<std::string> readFileLines(const std::string& path)
    {
        std::vector<std::string> lines;
        std::error_code ec;
        if (!fs::exists(path, ec) || ec)
        {
            Console::out::warning("Filesystem", "Failed to find file: " + path);
            return lines;
        }

        std::ifstream fileStream(path, std::ios::in);
        if (!fileStream.is_open())
        {
            Console::out::warning("Filesystem", "Failed to open file: " + path);
            return lines;
        }

        std::string line;
        while (std::getline(fileStream, line))
        {
            lines.push_back(line);
        }
        fileStream.close();
        return lines;
    }

    std::vector<std::string> getFolders(const std::string& path)
    {
        std::vector<std::string> folders;
        std::error_code ec;

        if (!fs::exists(path, ec) || ec)
        {
            Console::out::warning("Filesystem", "Path does not exist: " + path);
            return folders;
        }

        for (const auto& entry : fs::directory_iterator(path, ec))
        {
            if (ec)
            {
                Console::out::warning("Filesystem", "Error reading directory entry: " + ec.message());
                continue;
            }

            if (fs::is_directory(entry, ec) && !ec)
            {
                folders.push_back(entry.path().filename().string());
            }
        }

        if (ec)
        {
            Console::out::warning("Filesystem", "Error accessing path: " + ec.message());
        }

        return folders;
    }

    std::string getFilename(const std::string& path)
    {
        fs::path fsPath(path);
        return fsPath.filename().string();
    }

    std::string getFilenameRaw(const std::string& path)
    {
        fs::path fsPath(path);
        return fsPath.stem().string();
    }

    std::string getFileExtension(const std::string& filename)
    {
        fs::path fsPath(filename);
        std::string ext = fsPath.extension().string();

        // Return the extension only if it's not empty (matches original behavior)
        return (!ext.empty()) ? ext : "";
    }

    std::vector<std::string> getFilesWithExtensions(const std::string& path, const std::vector<std::string>& extensions)
    {
        std::vector<std::string> files;
        std::error_code ec;

        // Check if the path exists and is a directory
        if (!fs::exists(path, ec) || ec)
        {
            Console::out::warning("Filesystem", "Path does not exist: '" + path + "'");
            return files;
        }

        if (!fs::is_directory(path, ec) || ec)
        {
            Console::out::warning("Filesystem", "Path is not a directory: '" + path + "'");
            return files;
        }

        // Iterate through the directory and subdirectories
        fs::recursive_directory_iterator dirIter(path, fs::directory_options::skip_permission_denied, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Error accessing directory: " + ec.message());
            return files;
        }

        fs::recursive_directory_iterator end;
        while (dirIter != end)
        {
            std::error_code iterEc;
            if (fs::is_regular_file(*dirIter, iterEc) && !iterEc)
            {
                std::string filePath = dirIter->path().string();
                // Get file extension and convert to lowercase for case-insensitive matching
                std::string ext = dirIter->path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower); // make extension lowercase

                for (const auto& validExt : extensions)
                {
                    std::string lowerValidExt = validExt;
                    std::transform(lowerValidExt.begin(), lowerValidExt.end(), lowerValidExt.begin(), ::tolower);
                    if (ext == lowerValidExt) // Check if the file extension matches
                    {
                        files.push_back(filePath);
                        break; // No need to check further extensions if we found a match
                    }
                }
            }

            // Safe increment with error handling
            dirIter.increment(iterEc);
            if (iterEc)
            {
                Console::out::warning("Filesystem", "Error traversing directory: " + iterEc.message());
                // Try to skip problematic directory
                if (dirIter != end)
                {
                    dirIter++;
                }
            }
        }

        return files;
    }
}