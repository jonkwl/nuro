//
// This is Nuros filesystem API wrapper.
// All exceptions are internally handled to allow safe usage without requiring external exception management.
//

#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <optional>
#include <filesystem>

namespace FS
{

    //
    // FILESYSTEM TYPE ALIASES
    //

    using Path = std::filesystem::path;
    using DirectoryIterator = std::filesystem::directory_iterator;
    using RecursiveDirectoryIterator = std::filesystem::recursive_directory_iterator;
    using DirectoryEntry = std::filesystem::directory_entry;
    using FileStatus = std::filesystem::file_status;
    using SpaceInfo = std::filesystem::space_info;
    using FileType = std::filesystem::file_type;
    using Perms = std::filesystem::perms;
    using Perm_options = std::filesystem::perm_options;
    using Copy_options = std::filesystem::copy_options;
    using FileTime = std::filesystem::file_time_type;
    using FilesystemError = std::filesystem::filesystem_error;
    using Directory_options = std::filesystem::directory_options;
    using PathFormat = std::filesystem::path::format;
    using DirectoryOptions = std::filesystem::directory_options;
    using RecursiveDirectoryOptions = std::filesystem::directory_options;
    using SymlinkStatus = std::filesystem::file_status;
    using FileSize = std::uintmax_t;

    //
    // FILE ACCESS & MANIPULATION
    //

    // Reads entire file contents as string
    std::string readFile(const std::filesystem::path& path);

    // Reads file line by line
    std::vector<std::string> readFileLines(const std::filesystem::path& path);

    // Writes string content to file (overwrites existing content)
    bool writeFile(const std::filesystem::path& path, const std::string& content);

    // Appends string content to existing file
    bool appendToFile(const std::filesystem::path& path, const std::string& content);

    // Copies a file, optionally overwriting if destination exists
    bool copyFile(const std::filesystem::path& from, const std::filesystem::path& to, bool overwriteExisting = true);

    //
    // PATH INFORMATION
    //

    // Returns absolute path from relative path
    std::filesystem::path getAbsolutePath(const std::filesystem::path& path);

    // Returns canonical path (resolves symlinks, . and ..)
    std::filesystem::path getCanonicalPath(const std::filesystem::path& path);

    // Returns relative path from base to path
    std::filesystem::path getRelativePath(const std::filesystem::path& path, const std::filesystem::path& base);

    //
    // FILE STATUS
    //

    // Checks if path exists
    bool exists(const std::filesystem::path& path);

    // Checks if path is a regular file
    bool isRegularFile(const std::filesystem::path& path);

    // Checks if path is a directory
    bool isDirectory(const std::filesystem::path& path);

    // Checks if path is a symbolic link
    bool isSymlink(const std::filesystem::path& path);

    // Checks if path is empty (directory with no entries or zero-length file)
    bool isEmpty(const std::filesystem::path& path);

    // Returns file size in bytes, 0 for non-files or errors
    uintmax_t fileSize(const std::filesystem::path& path);

    //
    // FILE TIMES
    //

    // Gets last write time of file or directory
    std::optional<std::filesystem::file_time_type> getLastWriteTime(const std::filesystem::path& path);

    // Sets last write time for file or directory
    bool setLastWriteTime(const std::filesystem::path& path, std::filesystem::file_time_type newTime);

    //
    // DIRECTORY OPERATIONS
    //

    // Creates directory (single level)
    bool createDirectory(const std::filesystem::path& path);

    // Creates directories recursively (like mkdir -p)
    bool createDirectories(const std::filesystem::path& path);

    // Removes file or empty directory
    bool remove(const std::filesystem::path& path);

    // Removes directory and all contents recursively
    bool removeAll(const std::filesystem::path& path);

    // Returns all directory entries (files and subdirectories)
    std::vector<std::filesystem::path> getDirectoryEntries(const std::filesystem::path& path);

    // Returns only directories within a path
    std::vector<std::filesystem::path> getFolders(const std::filesystem::path& path);

    // Returns only files within a path
    std::vector<std::filesystem::path> getFiles(const std::filesystem::path& path);

    // Returns files with specific extensions (recursive)
    std::vector<std::filesystem::path> getFilesWithExtensions(const std::filesystem::path& path, const std::vector<std::filesystem::path>& extensions);

    // Returns files with specific extension (recursive)
    std::vector<std::filesystem::path> getFilesWithExtension(const std::filesystem::path& path, const std::filesystem::path& extension);

    // Returns files matching a pattern
    std::vector<std::filesystem::path> getFilesMatching(const std::filesystem::path& path, const std::string& pattern, bool recursive = true);

    //
    // PATH MODIFICATIONS
    //

    // Renames/moves a file or directory
    bool rename(const std::filesystem::path& from, const std::filesystem::path& to);

    // Creates a symlink
    bool createSymlink(const std::filesystem::path& target, const std::filesystem::path& link);

    // Creates a hard link
    bool createHardLink(const std::filesystem::path& target, const std::filesystem::path& link);

    // Creates a directory symlink
    bool createDirectorySymlink(const std::filesystem::path& target, const std::filesystem::path& link);

    // Reads target of symlink
    std::filesystem::path readSymlink(const std::filesystem::path& path);

    // Gets current path
    std::filesystem::path currentPath();

    // Sets current path, returns success
    bool setCurrentPath(const std::filesystem::path& path);

    //
    // SPACE INFO
    //

    // Gets free space on volume containing path
    uintmax_t getFreeSpace(const std::filesystem::path& path);

    // Gets total capacity of volume containing path
    uintmax_t getTotalSpace(const std::filesystem::path& path);

    // Gets space available to calling process on volume containing path
    uintmax_t getAvailableSpace(const std::filesystem::path& path);

    //
    // TMP DIRECTORIES
    //

    // Gets system's temp directory
    std::filesystem::path getTempDirectory();

    // Creates unique temporary directory and returns its path
    std::filesystem::path createTempDirectory(const std::filesystem::path& model = "%%%%-%%%%-%%%%-%%%%");

    // Creates unique temporary filename and returns its path
    std::filesystem::path createTempFilename(const std::filesystem::path& model = "%%%%-%%%%-%%%%-%%%%");

    //
    // PATH NORMALIZATION
    //

    // Normalizes path separators to system native
    std::filesystem::path normalizeSeparators(const std::filesystem::path& path);

    //
    // OTHER UTILITIES
    //

    // Touches a file (creates if doesn't exist, updates timestamp if it does)
    bool touch(const std::filesystem::path& path);

    // Checks if path has specific extension (case insensitive)
    bool hasExtension(const std::filesystem::path& path, const std::filesystem::path& extension);
}