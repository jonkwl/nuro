#include "fsutil.h"

#include <ctime>
#include <regex>
#include <random>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <utils/console.h>

namespace FSUtil
{

    std::string readFile(const fs::path& path)
    {
        std::error_code ec;
        if (!fs::exists(path, ec) || ec)
        {
            Console::out::warning("Filesystem", "Failed to find file: " + path.string());
            return "";
        }

        std::ifstream fileStream(path, std::ios::in | std::ios::binary);
        if (!fileStream.is_open())
        {
            Console::out::warning("Filesystem", "Failed to open file: " + path.string());
            return "";
        }

        std::stringstream buffer;
        buffer << fileStream.rdbuf();
        fileStream.close();
        return buffer.str();
    }

    std::vector<std::string> readFileLines(const fs::path& path)
    {
        std::vector<std::string> lines;
        std::error_code ec;
        if (!fs::exists(path, ec) || ec)
        {
            Console::out::warning("Filesystem", "Failed to find file: " + path.string());
            return lines;
        }

        std::ifstream fileStream(path, std::ios::in);
        if (!fileStream.is_open())
        {
            Console::out::warning("Filesystem", "Failed to open file: " + path.string());
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

    bool writeFile(const fs::path& path, const std::string& content)
    {
        try
        {
            // Ensure parent directory exists
            std::error_code ec;
            fs::path parent = path.parent_path();
            if (!parent.empty() && !fs::exists(parent, ec))
            {
                if (!createDirectories(parent))
                {
                    return false;
                }
            }

            std::ofstream fileStream(path, std::ios::out | std::ios::binary | std::ios::trunc);
            if (!fileStream.is_open())
            {
                Console::out::warning("Filesystem", "Failed to open file for writing: " + path.string());
                return false;
            }

            fileStream.write(content.data(), content.size());

            if (fileStream.fail())
            {
                Console::out::warning("Filesystem", "Failed to write data to file: " + path.string());
                return false;
            }

            fileStream.close();
            return true;
        }
        catch (const std::exception& e)
        {
            Console::out::warning("Filesystem", "Exception writing file " + path.string() + ": " + e.what());
            return false;
        }
    }

    bool appendToFile(const fs::path& path, const std::string& content)
    {
        try
        {
            // Ensure parent directory exists
            std::error_code ec;
            fs::path parent = path.parent_path();
            if (!parent.empty() && !fs::exists(parent, ec))
            {
                if (!createDirectories(parent))
                {
                    return false;
                }
            }

            std::ofstream fileStream(path, std::ios::out | std::ios::binary | std::ios::app);
            if (!fileStream.is_open())
            {
                Console::out::warning("Filesystem", "Failed to open file for appending: " + path.string());
                return false;
            }

            fileStream.write(content.data(), content.size());

            if (fileStream.fail())
            {
                Console::out::warning("Filesystem", "Failed to append data to file: " + path.string());
                return false;
            }

            fileStream.close();
            return true;
        }
        catch (const std::exception& e)
        {
            Console::out::warning("Filesystem", "Exception appending to file " + path.string() + ": " + e.what());
            return false;
        }
    }

    bool copyFile(const fs::path& from, const fs::path& to, bool overwriteExisting)
    {
        std::error_code ec;

        // Check source exists
        if (!fs::exists(from, ec))
        {
            Console::out::warning("Filesystem", "Source file does not exist: " + from.string());
            return false;
        }

        if (ec)
        {
            Console::out::warning("Filesystem", "Error checking source file: " + ec.message());
            return false;
        }

        // Create destination directory if needed
        fs::path toParent = to.parent_path();
        if (!toParent.empty() && !fs::exists(toParent, ec))
        {
            if (!createDirectories(toParent))
            {
                return false;
            }
        }

        // Set copy options
        fs::copy_options options = overwriteExisting ?
            fs::copy_options::overwrite_existing :
            fs::copy_options::none;

        fs::copy_file(from, to, options, ec);

        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to copy " + from.string() + " to " + to.string() + ": " + ec.message());
            return false;
        }

        return true;
    }

    fs::path getAbsolutePath(const fs::path& path)
    {
        std::error_code ec;
        fs::path result = fs::absolute(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to get absolute path for " + path.string() + ": " + ec.message());
            return path; // Return original path on failure
        }
        return result;
    }

    fs::path getCanonicalPath(const fs::path& path)
    {
        std::error_code ec;

        // Check if path exists first (canonical requires existing path)
        if (!fs::exists(path, ec))
        {
            Console::out::warning("Filesystem", "Cannot get canonical path, path does not exist: " + path.string());
            return getAbsolutePath(path); // Return absolute path as fallback
        }

        fs::path result = fs::canonical(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to get canonical path for " + path.string() + ": " + ec.message());
            return getAbsolutePath(path); // Return absolute path on failure
        }
        return result;
    }

    fs::path getRelativePath(const fs::path& path, const fs::path& base)
    {
        std::error_code ec;
        fs::path result = fs::relative(path, base, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to get relative path from " + base.string() + " to " + path.string() + ": " + ec.message());
            return path; // Return original path on failure
        }
        return result;
    }

    bool exists(const fs::path& path)
    {
        std::error_code ec;
        bool result = fs::exists(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Error checking if path exists " + path.string() + ": " + ec.message());
            return false;
        }
        return result;
    }

    bool isRegularFile(const fs::path& path)
    {
        std::error_code ec;
        bool result = fs::is_regular_file(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Error checking if path is regular file " + path.string() + ": " + ec.message());
            return false;
        }
        return result;
    }

    bool isDirectory(const fs::path& path)
    {
        std::error_code ec;
        bool result = fs::is_directory(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Error checking if path is directory " + path.string() + ": " + ec.message());
            return false;
        }
        return result;
    }

    bool isSymlink(const fs::path& path)
    {
        std::error_code ec;
        bool result = fs::is_symlink(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Error checking if path is symlink " + path.string() + ": " + ec.message());
            return false;
        }
        return result;
    }

    bool isEmpty(const fs::path& path)
    {
        std::error_code ec;

        if (!fs::exists(path, ec))
        {
            Console::out::warning("Filesystem", "Path does not exist when checking if empty: " + path.string());
            return true; // Non-existent path is considered empty
        }

        if (ec)
        {
            Console::out::warning("Filesystem", "Error checking if path exists: " + ec.message());
            return true;
        }

        if (fs::is_directory(path, ec))
        {
            if (ec)
            {
                Console::out::warning("Filesystem", "Error checking if path is directory: " + ec.message());
                return true;
            }

            // Check if directory is empty
            return fs::begin(fs::directory_iterator(path, ec)) == fs::end(fs::directory_iterator());
        }
        else if (fs::is_regular_file(path, ec))
        {
            if (ec)
            {
                Console::out::warning("Filesystem", "Error checking if path is file: " + ec.message());
                return true;
            }

            // Check if file is empty
            return fs::file_size(path, ec) == 0;
        }

        return true; // Other types (symlinks, etc.) are considered empty
    }

    uintmax_t fileSize(const fs::path& path)
    {
        std::error_code ec;

        if (!fs::is_regular_file(path, ec))
        {
            if (ec)
            {
                Console::out::warning("Filesystem", "Error checking if path is file: " + ec.message());
            }
            else
            {
                Console::out::warning("Filesystem", "Cannot get size: path is not a regular file: " + path.string());
            }
            return 0;
        }

        uintmax_t size = fs::file_size(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to get file size for " + path.string() + ": " + ec.message());
            return 0;
        }
        return size;
    }

    std::optional<fs::file_time_type> getLastWriteTime(const fs::path& path)
    {
        std::error_code ec;

        if (!fs::exists(path, ec))
        {
            Console::out::warning("Filesystem", "Path does not exist when getting last write time: " + path.string());
            return std::nullopt;
        }

        fs::file_time_type time = fs::last_write_time(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to get last write time for " + path.string() + ": " + ec.message());
            return std::nullopt;
        }
        return time;
    }

    bool setLastWriteTime(const fs::path& path, fs::file_time_type newTime)
    {
        std::error_code ec;

        if (!fs::exists(path, ec))
        {
            Console::out::warning("Filesystem", "Path does not exist when setting last write time: " + path.string());
            return false;
        }

        fs::last_write_time(path, newTime, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to set last write time for " + path.string() + ": " + ec.message());
            return false;
        }
        return true;
    }

    bool createDirectory(const fs::path& path)
    {
        std::error_code ec;

        bool result = fs::create_directory(path, ec);
        if (ec && ec.value() != 0)  // Ignore if directory already exists
        {
            Console::out::warning("Filesystem", "Failed to create directory " + path.string() + ": " + ec.message());
            return false;
        }
        return true;
    }

    bool createDirectories(const fs::path& path)
    {
        std::error_code ec;

        bool result = fs::create_directories(path, ec);
        if (ec && ec.value() != 0)  // Ignore if directory already exists
        {
            Console::out::warning("Filesystem", "Failed to create directories " + path.string() + ": " + ec.message());
            return false;
        }
        return true;
    }

    bool remove(const fs::path& path)
    {
        std::error_code ec;

        if (!fs::exists(path, ec))
        {
            // Not an error if the file doesn't exist
            return true;
        }

        bool result = fs::remove(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to remove " + path.string() + ": " + ec.message());
            return false;
        }
        return result;
    }

    bool removeAll(const fs::path& path)
    {
        std::error_code ec;

        if (!fs::exists(path, ec))
        {
            // Not an error if the directory doesn't exist
            return true;
        }

        uintmax_t count = fs::remove_all(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to remove directory and contents " + path.string() + ": " + ec.message());
            return false;
        }
        return true;
    }

    std::vector<fs::path> getDirectoryEntries(const fs::path& path)
    {
        std::vector<fs::path> entries;
        std::error_code ec;

        if (!fs::exists(path, ec) || !fs::is_directory(path, ec))
        {
            Console::out::warning("Filesystem", "Path does not exist or is not a directory: " + path.string());
            return entries;
        }

        for (const auto& entry : fs::directory_iterator(path, ec))
        {
            if (ec)
            {
                Console::out::warning("Filesystem", "Error reading directory entry: " + ec.message());
                continue;
            }
            entries.push_back(entry.path());
        }

        return entries;
    }

    std::vector<fs::path> getFolders(const fs::path& path)
    {
        std::vector<fs::path> folders;
        std::error_code ec;

        if (!fs::exists(path, ec) || !fs::is_directory(path, ec))
        {
            Console::out::warning("Filesystem", "Path does not exist or is not a directory: " + path.string());
            return folders;
        }

        for (const auto& entry : fs::directory_iterator(path, ec))
        {
            if (ec)
            {
                Console::out::warning("Filesystem", "Error reading directory entry: " + ec.message());
                continue;
            }

            if (fs::is_directory(entry, ec))
            {
                if (ec)
                {
                    Console::out::warning("Filesystem", "Error checking if entry is directory: " + ec.message());
                    continue;
                }
                folders.push_back(entry.path());
            }
        }

        return folders;
    }

    std::vector<fs::path> getFiles(const fs::path& path)
    {
        std::vector<fs::path> files;
        std::error_code ec;

        if (!fs::exists(path, ec) || !fs::is_directory(path, ec))
        {
            Console::out::warning("Filesystem", "Path does not exist or is not a directory: " + path.string());
            return files;
        }

        for (const auto& entry : fs::directory_iterator(path, ec))
        {
            if (ec)
            {
                Console::out::warning("Filesystem", "Error reading directory entry: " + ec.message());
                continue;
            }

            if (fs::is_regular_file(entry, ec))
            {
                if (ec)
                {
                    Console::out::warning("Filesystem", "Error checking if entry is file: " + ec.message());
                    continue;
                }
                files.push_back(entry.path());
            }
        }

        return files;
    }

    std::vector<fs::path> getFilesWithExtensions(const fs::path& path, const std::vector<fs::path>& extensions)
    {
        std::vector<fs::path> files;
        std::error_code ec;

        if (!fs::exists(path, ec) || !fs::is_directory(path, ec))
        {
            Console::out::warning("Filesystem", "Path does not exist or is not a directory: " + path.string());
            return files;
        }

        try
        {
            // Convert all extensions to lowercase for case-insensitive matching
            std::vector<std::string> lowerExtensions;
            for (const auto& ext : extensions)
            {
                std::string extStr = ext.string();
                std::transform(extStr.begin(), extStr.end(), extStr.begin(), ::tolower);
                lowerExtensions.push_back(extStr);
            }

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
                    fs::path filePath = dirIter->path();
                    std::string ext = filePath.extension().string();
                    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                    for (const auto& validExt : lowerExtensions)
                    {
                        if (ext == validExt)
                        {
                            files.push_back(filePath);
                            break;
                        }
                    }
                }

                // Safe increment with error handling
                dirIter.increment(iterEc);
                if (iterEc)
                {
                    Console::out::warning("Filesystem", "Error traversing directory: " + iterEc.message());
                    // Skip problem directory
                    if (dirIter != end)
                    {
                        dirIter++;
                    }
                }
            }
        }
        catch (const std::exception& e)
        {
            Console::out::warning("Filesystem", "Exception while searching for files with extensions: " + std::string(e.what()));
        }

        return files;
    }

    std::vector<fs::path> getFilesWithExtension(const fs::path& path, const fs::path& extension)
    {
        return getFilesWithExtensions(path, { extension });
    }

    std::vector<fs::path> getFilesMatching(const fs::path& path, const std::string& pattern, bool recursive)
    {
        std::vector<fs::path> files;
        std::error_code ec;

        if (!fs::exists(path, ec) || !fs::is_directory(path, ec))
        {
            Console::out::warning("Filesystem", "Path does not exist or is not a directory: " + path.string());
            return files;
        }

        try
        {
            // Compile regex pattern
            std::regex re;
            try {
                re = std::regex(pattern, std::regex::ECMAScript);
            }
            catch (const std::regex_error& e) {
                Console::out::warning("Filesystem", "Invalid regex pattern: " + std::string(e.what()));
                return files;
            }

            // Choose directory iterator based on recursive flag
            if (recursive)
            {
                fs::recursive_directory_iterator dirIter(path, fs::directory_options::skip_permission_denied, ec);
                if (ec)
                {
                    Console::out::warning("Filesystem", "Error accessing directory: " + path.string() + ": " + ec.message());
                    return files;
                }

                fs::recursive_directory_iterator end;
                while (dirIter != end)
                {
                    std::error_code iterEc;

                    if (fs::is_regular_file(*dirIter, iterEc) && !iterEc)
                    {
                        std::string filename = dirIter->path().filename().string();
                        if (std::regex_match(filename, re))
                        {
                            files.push_back(dirIter->path());
                        }
                    }

                    // Safe increment with error handling
                    dirIter.increment(iterEc);
                    if (iterEc)
                    {
                        Console::out::warning("Filesystem", "Error traversing directory: " + iterEc.message());
                        // Skip problem directory
                        if (dirIter != end)
                        {
                            dirIter++;
                        }
                    }
                }
            }
            else
            {
                fs::directory_iterator dirIter(path, ec);
                if (ec)
                {
                    Console::out::warning("Filesystem", "Error accessing directory: " + path.string() + ": " + ec.message());
                    return files;
                }

                for (const auto& entry : dirIter)
                {
                    std::error_code iterEc;

                    if (fs::is_regular_file(entry, iterEc) && !iterEc)
                    {
                        std::string filename = entry.path().filename().string();
                        if (std::regex_match(filename, re))
                        {
                            files.push_back(entry.path());
                        }
                    }
                }
            }
        }
        catch (const std::exception& e)
        {
            Console::out::warning("Filesystem", "Exception while searching for files matching pattern: " + std::string(e.what()));
        }

        return files;
    }

    // PATH MODIFICATION TO BE DONE

}