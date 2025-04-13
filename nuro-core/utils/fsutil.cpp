#include "fsutil.h"

#include <ctime>
#include <regex>
#include <random>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <utils/console.h>

namespace sfs = std::filesystem;

namespace FS
{

    std::string readFile(const sfs::path& path)
    {
        std::error_code ec;
        if (!sfs::exists(path, ec) || ec)
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

    std::vector<std::string> readFileLines(const sfs::path& path)
    {
        std::vector<std::string> lines;
        std::error_code ec;
        if (!sfs::exists(path, ec) || ec)
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

    bool writeFile(const sfs::path& path, const std::string& content)
    {
        try
        {
            // Ensure parent directory exists
            std::error_code ec;
            sfs::path parent = path.parent_path();
            if (!parent.empty() && !sfs::exists(parent, ec))
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

    bool appendToFile(const sfs::path& path, const std::string& content)
    {
        try
        {
            // Ensure parent directory exists
            std::error_code ec;
            sfs::path parent = path.parent_path();
            if (!parent.empty() && !sfs::exists(parent, ec))
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

    bool copyFile(const sfs::path& from, const sfs::path& to, bool overwriteExisting)
    {
        std::error_code ec;

        // Check source exists
        if (!sfs::exists(from, ec))
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
        sfs::path toParent = to.parent_path();
        if (!toParent.empty() && !sfs::exists(toParent, ec))
        {
            if (!createDirectories(toParent))
            {
                return false;
            }
        }

        // Set copy options
        sfs::copy_options options = overwriteExisting ?
            sfs::copy_options::overwrite_existing :
            sfs::copy_options::none;

        sfs::copy_file(from, to, options, ec);

        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to copy " + from.string() + " to " + to.string() + ": " + ec.message());
            return false;
        }

        return true;
    }

    sfs::path getAbsolutePath(const sfs::path& path)
    {
        std::error_code ec;
        sfs::path result = sfs::absolute(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to get absolute path for " + path.string() + ": " + ec.message());
            return path; // Return original path on failure
        }
        return result;
    }

    sfs::path getCanonicalPath(const sfs::path& path)
    {
        std::error_code ec;

        // Check if path exists first (canonical requires existing path)
        if (!sfs::exists(path, ec))
        {
            Console::out::warning("Filesystem", "Cannot get canonical path, path does not exist: " + path.string());
            return getAbsolutePath(path); // Return absolute path as fallback
        }

        sfs::path result = sfs::canonical(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to get canonical path for " + path.string() + ": " + ec.message());
            return getAbsolutePath(path); // Return absolute path on failure
        }
        return result;
    }

    sfs::path getRelativePath(const sfs::path& path, const sfs::path& base)
    {
        std::error_code ec;
        sfs::path result = sfs::relative(path, base, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to get relative path from " + base.string() + " to " + path.string() + ": " + ec.message());
            return path; // Return original path on failure
        }
        return result;
    }

    bool exists(const sfs::path& path)
    {
        std::error_code ec;
        bool result = sfs::exists(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Error checking if path exists " + path.string() + ": " + ec.message());
            return false;
        }
        return result;
    }

    bool isRegularFile(const sfs::path& path)
    {
        std::error_code ec;
        bool result = sfs::is_regular_file(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Error checking if path is regular file " + path.string() + ": " + ec.message());
            return false;
        }
        return result;
    }

    bool isDirectory(const sfs::path& path)
    {
        std::error_code ec;
        bool result = sfs::is_directory(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Error checking if path is directory " + path.string() + ": " + ec.message());
            return false;
        }
        return result;
    }

    bool isSymlink(const sfs::path& path)
    {
        std::error_code ec;
        bool result = sfs::is_symlink(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Error checking if path is symlink " + path.string() + ": " + ec.message());
            return false;
        }
        return result;
    }

    bool isEmpty(const sfs::path& path)
    {
        std::error_code ec;

        if (!sfs::exists(path, ec))
        {
            Console::out::warning("Filesystem", "Path does not exist when checking if empty: " + path.string());
            return true; // Non-existent path is considered empty
        }

        if (ec)
        {
            Console::out::warning("Filesystem", "Error checking if path exists: " + ec.message());
            return true;
        }

        if (sfs::is_directory(path, ec))
        {
            if (ec)
            {
                Console::out::warning("Filesystem", "Error checking if path is directory: " + ec.message());
                return true;
            }

            // Check if directory is empty
            return sfs::begin(sfs::directory_iterator(path, ec)) == sfs::end(sfs::directory_iterator());
        }
        else if (sfs::is_regular_file(path, ec))
        {
            if (ec)
            {
                Console::out::warning("Filesystem", "Error checking if path is file: " + ec.message());
                return true;
            }

            // Check if file is empty
            return sfs::file_size(path, ec) == 0;
        }

        return true; // Other types (symlinks, etc.) are considered empty
    }

    uintmax_t fileSize(const sfs::path& path)
    {
        std::error_code ec;

        if (!sfs::is_regular_file(path, ec))
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

        uintmax_t size = sfs::file_size(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to get file size for " + path.string() + ": " + ec.message());
            return 0;
        }
        return size;
    }

    std::optional<sfs::file_time_type> getLastWriteTime(const sfs::path& path)
    {
        std::error_code ec;

        if (!sfs::exists(path, ec))
        {
            Console::out::warning("Filesystem", "Path does not exist when getting last write time: " + path.string());
            return std::nullopt;
        }

        sfs::file_time_type time = sfs::last_write_time(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to get last write time for " + path.string() + ": " + ec.message());
            return std::nullopt;
        }
        return time;
    }

    bool setLastWriteTime(const sfs::path& path, sfs::file_time_type newTime)
    {
        std::error_code ec;

        if (!sfs::exists(path, ec))
        {
            Console::out::warning("Filesystem", "Path does not exist when setting last write time: " + path.string());
            return false;
        }

        sfs::last_write_time(path, newTime, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to set last write time for " + path.string() + ": " + ec.message());
            return false;
        }
        return true;
    }

    bool createDirectory(const sfs::path& path)
    {
        std::error_code ec;

        bool result = sfs::create_directory(path, ec);
        if (ec && ec.value() != 0)  // Ignore if directory already exists
        {
            Console::out::warning("Filesystem", "Failed to create directory " + path.string() + ": " + ec.message());
            return false;
        }
        return true;
    }

    bool createDirectories(const sfs::path& path)
    {
        std::error_code ec;

        bool result = sfs::create_directories(path, ec);
        if (ec && ec.value() != 0)  // Ignore if directory already exists
        {
            Console::out::warning("Filesystem", "Failed to create directories " + path.string() + ": " + ec.message());
            return false;
        }
        return true;
    }

    bool remove(const sfs::path& path)
    {
        std::error_code ec;

        if (!sfs::exists(path, ec))
        {
            // Not an error if the file doesn't exist
            return true;
        }

        bool result = sfs::remove(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to remove " + path.string() + ": " + ec.message());
            return false;
        }
        return result;
    }

    bool removeAll(const sfs::path& path)
    {
        std::error_code ec;

        if (!sfs::exists(path, ec))
        {
            // Not an error if the directory doesn't exist
            return true;
        }

        uintmax_t count = sfs::remove_all(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to remove directory and contents " + path.string() + ": " + ec.message());
            return false;
        }
        return true;
    }

    std::vector<sfs::path> getDirectoryEntries(const sfs::path& path)
    {
        std::vector<sfs::path> entries;
        std::error_code ec;

        if (!sfs::exists(path, ec) || !sfs::is_directory(path, ec))
        {
            Console::out::warning("Filesystem", "Path does not exist or is not a directory: " + path.string());
            return entries;
        }

        for (const auto& entry : sfs::directory_iterator(path, ec))
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

    std::vector<sfs::path> getFolders(const sfs::path& path)
    {
        std::vector<sfs::path> folders;
        std::error_code ec;

        if (!sfs::exists(path, ec) || !sfs::is_directory(path, ec))
        {
            Console::out::warning("Filesystem", "Path does not exist or is not a directory: " + path.string());
            return folders;
        }

        for (const auto& entry : sfs::directory_iterator(path, ec))
        {
            if (ec)
            {
                Console::out::warning("Filesystem", "Error reading directory entry: " + ec.message());
                continue;
            }

            if (sfs::is_directory(entry, ec))
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

    std::vector<sfs::path> getFiles(const sfs::path& path)
    {
        std::vector<sfs::path> files;
        std::error_code ec;

        if (!sfs::exists(path, ec) || !sfs::is_directory(path, ec))
        {
            Console::out::warning("Filesystem", "Path does not exist or is not a directory: " + path.string());
            return files;
        }

        for (const auto& entry : sfs::directory_iterator(path, ec))
        {
            if (ec)
            {
                Console::out::warning("Filesystem", "Error reading directory entry: " + ec.message());
                continue;
            }

            if (sfs::is_regular_file(entry, ec))
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

    std::vector<sfs::path> getFilesWithExtensions(const sfs::path& path, const std::vector<sfs::path>& extensions)
    {
        std::vector<sfs::path> files;
        std::error_code ec;

        if (!sfs::exists(path, ec) || !sfs::is_directory(path, ec))
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

            sfs::recursive_directory_iterator dirIter(path, sfs::directory_options::skip_permission_denied, ec);
            if (ec)
            {
                Console::out::warning("Filesystem", "Error accessing directory: " + ec.message());
                return files;
            }

            sfs::recursive_directory_iterator end;
            while (dirIter != end)
            {
                std::error_code iterEc;

                if (sfs::is_regular_file(*dirIter, iterEc) && !iterEc)
                {
                    sfs::path filePath = dirIter->path();
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

    std::vector<sfs::path> getFilesWithExtension(const sfs::path& path, const sfs::path& extension)
    {
        return getFilesWithExtensions(path, { extension });
    }

    std::vector<sfs::path> getFilesMatching(const sfs::path& path, const std::string& pattern, bool recursive)
    {
        std::vector<sfs::path> files;
        std::error_code ec;

        if (!sfs::exists(path, ec) || !sfs::is_directory(path, ec))
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
                sfs::recursive_directory_iterator dirIter(path, sfs::directory_options::skip_permission_denied, ec);
                if (ec)
                {
                    Console::out::warning("Filesystem", "Error accessing directory: " + path.string() + ": " + ec.message());
                    return files;
                }

                sfs::recursive_directory_iterator end;
                while (dirIter != end)
                {
                    std::error_code iterEc;

                    if (sfs::is_regular_file(*dirIter, iterEc) && !iterEc)
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
                sfs::directory_iterator dirIter(path, ec);
                if (ec)
                {
                    Console::out::warning("Filesystem", "Error accessing directory: " + path.string() + ": " + ec.message());
                    return files;
                }

                for (const auto& entry : dirIter)
                {
                    std::error_code iterEc;

                    if (sfs::is_regular_file(entry, iterEc) && !iterEc)
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

    bool rename(const sfs::path& from, const sfs::path& to)
    {
        std::error_code ec;

        if (!sfs::exists(from, ec))
        {
            Console::out::warning("Filesystem", "Source path does not exist: " + from.string());
            return false;
        }

        // Create parent directory if it doesn't exist
        sfs::path toParent = to.parent_path();
        if (!toParent.empty() && !sfs::exists(toParent, ec))
        {
            if (!createDirectories(toParent))
            {
                return false;
            }
        }

        sfs::rename(from, to, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to rename " + from.string() + " to " + to.string() + ": " + ec.message());
            return false;
        }

        return true;
    }

    bool createSymlink(const sfs::path& target, const sfs::path& link)
    {
        std::error_code ec;

        // Create parent directory if it doesn't exist
        sfs::path linkParent = link.parent_path();
        if (!linkParent.empty() && !sfs::exists(linkParent, ec))
        {
            if (!createDirectories(linkParent))
            {
                return false;
            }
        }

        sfs::create_symlink(target, link, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to create symlink from " + link.string() + " to " + target.string() + ": " + ec.message());
            return false;
        }

        return true;
    }

    bool createHardLink(const sfs::path& target, const sfs::path& link)
    {
        std::error_code ec;

        if (!sfs::exists(target, ec))
        {
            Console::out::warning("Filesystem", "Target file does not exist: " + target.string());
            return false;
        }

        // Create parent directory if it doesn't exist
        sfs::path linkParent = link.parent_path();
        if (!linkParent.empty() && !sfs::exists(linkParent, ec))
        {
            if (!createDirectories(linkParent))
            {
                return false;
            }
        }

        sfs::create_hard_link(target, link, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to create hard link from " + link.string() + " to " + target.string() + ": " + ec.message());
            return false;
        }

        return true;
    }

    bool createDirectorySymlink(const sfs::path& target, const sfs::path& link)
    {
        std::error_code ec;

        // Create parent directory if it doesn't exist
        sfs::path linkParent = link.parent_path();
        if (!linkParent.empty() && !sfs::exists(linkParent, ec))
        {
            if (!createDirectories(linkParent))
            {
                return false;
            }
        }

        sfs::create_directory_symlink(target, link, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to create directory symlink from " + link.string() + " to " + target.string() + ": " + ec.message());
            return false;
        }

        return true;
    }

    sfs::path readSymlink(const sfs::path& path)
    {
        std::error_code ec;

        if (!sfs::exists(path, ec))
        {
            Console::out::warning("Filesystem", "Symlink does not exist: " + path.string());
            return sfs::path();
        }

        if (!sfs::is_symlink(path, ec))
        {
            Console::out::warning("Filesystem", "Path is not a symlink: " + path.string());
            return sfs::path();
        }

        sfs::path target = sfs::read_symlink(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to read symlink " + path.string() + ": " + ec.message());
            return sfs::path();
        }

        return target;
    }

    sfs::path currentPath()
    {
        std::error_code ec;
        sfs::path result = sfs::current_path(ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to get current path: " + ec.message());
            return sfs::path();
        }
        return result;
    }

    bool setCurrentPath(const sfs::path& path)
    {
        std::error_code ec;

        if (!sfs::exists(path, ec))
        {
            Console::out::warning("Filesystem", "Path does not exist: " + path.string());
            return false;
        }

        sfs::current_path(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to set current path to " + path.string() + ": " + ec.message());
            return false;
        }

        return true;
    }

    // Space Info
    // ---------

    uintmax_t getFreeSpace(const sfs::path& path)
    {
        std::error_code ec;

        sfs::space_info space = sfs::space(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to get space info for " + path.string() + ": " + ec.message());
            return 0;
        }

        return space.free;
    }

    uintmax_t getTotalSpace(const sfs::path& path)
    {
        std::error_code ec;

        sfs::space_info space = sfs::space(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to get space info for " + path.string() + ": " + ec.message());
            return 0;
        }

        return space.capacity;
    }

    uintmax_t getAvailableSpace(const sfs::path& path)
    {
        std::error_code ec;

        sfs::space_info space = sfs::space(path, ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to get space info for " + path.string() + ": " + ec.message());
            return 0;
        }

        return space.available;
    }

    // Temp Directories
    // ---------------

    sfs::path getTempDirectory()
    {
        std::error_code ec;
        sfs::path temp = sfs::temp_directory_path(ec);
        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to get temp directory path: " + ec.message());
            return sfs::path();
        }
        return temp;
    }

    sfs::path createTempDirectory(const sfs::path& model)
    {
        std::error_code ec;
        sfs::path tempDir = sfs::temp_directory_path(ec);

        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to get temp directory path: " + ec.message());
            return sfs::path();
        }

        // Generate a random string for the directory name
        std::string dirTemplate = model.string();

        // For safety, ensure the template contains at least one % for randomization
        if (dirTemplate.find('%') == std::string::npos)
        {
            dirTemplate += "-%%%%";
        }

        // Replace % with random chars
        static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);

        for (size_t i = 0; i < dirTemplate.length(); ++i)
        {
            if (dirTemplate[i] == '%')
            {
                dirTemplate[i] = alphanum[dis(gen)];
            }
        }

        sfs::path fullPath = tempDir / dirTemplate;

        // Ensure directory doesn't already exist
        if (sfs::exists(fullPath, ec))
        {
            Console::out::warning("Filesystem", "Temp directory already exists, generating new name");
            return createTempDirectory(model); // Recursively try again
        }

        // Create the directory
        if (!createDirectory(fullPath))
        {
            return sfs::path();
        }

        return fullPath;
    }

    sfs::path createTempFilename(const sfs::path& model)
    {
        std::error_code ec;
        sfs::path tempDir = sfs::temp_directory_path(ec);

        if (ec)
        {
            Console::out::warning("Filesystem", "Failed to get temp directory path: " + ec.message());
            return sfs::path();
        }

        // Generate a random string for the filename
        std::string fileTemplate = model.string();

        // For safety, ensure the template contains at least one % for randomization
        if (fileTemplate.find('%') == std::string::npos)
        {
            fileTemplate += "-%%%%";
        }

        // Replace % with random chars
        static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);

        for (size_t i = 0; i < fileTemplate.length(); ++i)
        {
            if (fileTemplate[i] == '%')
            {
                fileTemplate[i] = alphanum[dis(gen)];
            }
        }

        sfs::path fullPath = tempDir / fileTemplate;

        // Ensure file doesn't already exist
        if (sfs::exists(fullPath, ec))
        {
            return createTempFilename(model); // Recursively try again
        }

        return fullPath;
    }

    // Path Normalization
    // -----------------

    sfs::path normalizeSeparators(const sfs::path& path)
    {
        std::string pathStr = path.string();

        // Replace all forward slashes with preferred separator
        char nativeSep = sfs::path::preferred_separator;
        for (size_t i = 0; i < pathStr.length(); ++i)
        {
            if (pathStr[i] == '/' || pathStr[i] == '\\')
            {
                pathStr[i] = nativeSep;
            }
        }

        // Replace double separators with single ones
        std::string doubleSep(2, nativeSep);
        std::string singleSep(1, nativeSep);
        size_t pos = 0;
        while ((pos = pathStr.find(doubleSep, pos)) != std::string::npos)
        {
            pathStr.replace(pos, 2, singleSep);
        }

        return sfs::path(pathStr);
    }

    bool touch(const sfs::path& path)
    {
        std::error_code ec;

        // If file exists, update timestamp
        if (sfs::exists(path, ec))
        {
            auto now = sfs::file_time_type::clock::now();
            sfs::last_write_time(path, now, ec);
            if (ec)
            {
                Console::out::warning("Filesystem", "Failed to update timestamp for " + path.string() + ": " + ec.message());
                return false;
            }
            return true;
        }

        // Otherwise create empty file
        // Ensure parent directory exists
        sfs::path parent = path.parent_path();
        if (!parent.empty() && !sfs::exists(parent, ec))
        {
            if (!createDirectories(parent))
            {
                return false;
            }
        }

        // Create empty file
        std::ofstream file(path);
        if (!file.is_open())
        {
            Console::out::warning("Filesystem", "Failed to create file " + path.string());
            return false;
        }
        file.close();

        return true;
    }

    bool hasExtension(const sfs::path& path, const sfs::path& extension)
    {
        std::string pathExt = path.extension().string();
        std::string checkExt = extension.string();

        // Ensure extension starts with a dot if not already
        if (!checkExt.empty() && checkExt[0] != '.')
        {
            checkExt = "." + checkExt;
        }

        // Convert both to lowercase for case-insensitive comparison
        std::transform(pathExt.begin(), pathExt.end(), pathExt.begin(), ::tolower);
        std::transform(checkExt.begin(), checkExt.end(), checkExt.begin(), ::tolower);

        return pathExt == checkExt;
    }

}