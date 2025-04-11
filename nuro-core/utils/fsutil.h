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

namespace fs = std::filesystem;

using path = fs::path;

// todo: 
// 1) replace any fs::path with path later
// 2) add missing function implementations
// 3) consistently use fsutil wrapper functions throughout the codebase

namespace FSUtil
{
    //
    // FILE ACCESS & MANIPULATION
    //

    // Reads entire file contents as string
    std::string readFile(const fs::path& path);

    // Reads file line by line
    std::vector<std::string> readFileLines(const fs::path& path);

    // Writes string content to file (overwrites existing content)
    bool writeFile(const fs::path& path, const std::string& content);

    // Appends string content to existing file
    bool appendToFile(const fs::path& path, const std::string& content);

    // Copies a file, optionally overwriting if destination exists
    bool copyFile(const fs::path& from, const fs::path& to, bool overwriteExisting = true);

    //
    // PATH INFORMATION
    //

    // Returns absolute path from relative path
    fs::path getAbsolutePath(const fs::path& path);

    // Returns canonical path (resolves symlinks, . and ..)
    fs::path getCanonicalPath(const fs::path& path);

    // Returns relative path from base to path
    fs::path getRelativePath(const fs::path& path, const fs::path& base);

    //
    // FILE STATUS
    //

    // Checks if path exists
    bool exists(const fs::path& path);

    // Checks if path is a regular file
    bool isRegularFile(const fs::path& path);

    // Checks if path is a directory
    bool isDirectory(const fs::path& path);

    // Checks if path is a symbolic link
    bool isSymlink(const fs::path& path);

    // Checks if path is empty (directory with no entries or zero-length file)
    bool isEmpty(const fs::path& path);

    // Returns file size in bytes, 0 for non-files or errors
    uintmax_t fileSize(const fs::path& path);

    //
    // FILE TIMES
    //

    // Gets last write time of file or directory
    std::optional<fs::file_time_type> getLastWriteTime(const fs::path& path);

    // Sets last write time for file or directory
    bool setLastWriteTime(const fs::path& path, fs::file_time_type newTime);

    //
    // DIRECTORY OPERATIONS
    //

    // Creates directory (single level)
    bool createDirectory(const fs::path& path);

    // Creates directories recursively (like mkdir -p)
    bool createDirectories(const fs::path& path);

    // Removes file or empty directory
    bool remove(const fs::path& path);

    // Removes directory and all contents recursively
    bool removeAll(const fs::path& path);

    // Returns all directory entries (files and subdirectories)
    std::vector<fs::path> getDirectoryEntries(const fs::path& path);

    // Returns only directories within a path
    std::vector<fs::path> getFolders(const fs::path& path);

    // Returns only files within a path
    std::vector<fs::path> getFiles(const fs::path& path);

    // Returns files with specific extensions (recursive)
    std::vector<fs::path> getFilesWithExtensions(const fs::path& path, const std::vector<fs::path>& extensions);

    // Returns files with specific extension (recursive)
    std::vector<fs::path> getFilesWithExtension(const fs::path& path, const fs::path& extension);

    // Returns files matching a pattern
    std::vector<fs::path> getFilesMatching(const fs::path& path, const std::string& pattern, bool recursive = true);

    //
    // PATH MODIFICATIONS
    //

    // Renames/moves a file or directory
    bool rename(const fs::path& from, const fs::path& to);

    // Creates a symlink
    bool createSymlink(const fs::path& target, const fs::path& link);

    // Creates a hard link
    bool createHardLink(const fs::path& target, const fs::path& link);

    // Creates a directory symlink
    bool createDirectorySymlink(const fs::path& target, const fs::path& link);

    // Reads target of symlink
    fs::path readSymlink(const fs::path& path);

    // Gets current path
    fs::path currentPath();

    // Sets current path, returns success
    bool setCurrentPath(const fs::path& path);

    //
    // SPACE INFO
    //

    // Gets free space on volume containing path
    uintmax_t getFreeSpace(const fs::path& path);

    // Gets total capacity of volume containing path
    uintmax_t getTotalSpace(const fs::path& path);

    // Gets space available to calling process on volume containing path
    uintmax_t getAvailableSpace(const fs::path& path);

    //
    // TMP DIRECTORIES
    //

    // Gets system's temp directory
    fs::path getTempDirectory();

    // Creates unique temporary directory and returns its path
    fs::path createTempDirectory(const fs::path& model = "%%%%-%%%%-%%%%-%%%%");

    // Creates unique temporary filename and returns its path
    fs::path createTempFilename(const fs::path& model = "%%%%-%%%%-%%%%-%%%%");

    //
    // PATH NORMALIZATION
    //

    // Normalizes path separators to system native
    fs::path normalizeSeparators(const fs::path& path);

    //
    // OTHER UTILITIES
    //

    // Creates backup copy of file with .bak extension
    bool createBackup(const fs::path& path);

    // Touches a file (creates if doesn't exist, updates timestamp if it does)
    bool touch(const fs::path& path);

    // Checks if path has specific extension (case insensitive)
    bool hasExtension(const fs::path& path, const fs::path& extension);
}