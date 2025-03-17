#include "ioutils.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

#include <utils/console.h>

namespace IOUtils
{

	std::string readFile(const std::string& path)
	{
		std::ifstream fileStream(path);
		if (!fileStream.is_open())
		{
			std::cerr << "File Reader | Failed to open file: " << path << std::endl;
			return "";
		}
		std::stringstream buffer;
		buffer << fileStream.rdbuf();
		fileStream.close();
		return buffer.str();
	}

	std::vector<std::string> readFileLines(const std::string& path)
	{
		std::ifstream fileStream(path);
		std::vector<std::string> lines;
		if (!fileStream.is_open())
		{
			std::cerr << "File Reader | Failed to open file: " << path << std::endl;
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
		try
		{
			for (const auto& entry : std::filesystem::directory_iterator(path))
			{
				if (entry.is_directory())
				{
					folders.push_back(entry.path().filename().string());
				}
			}
		}
		catch (const std::filesystem::filesystem_error& e)
		{
			std::cerr << "File Reader | Error accessing path: " << e.what() << std::endl;
		}
		return folders;
	}

	std::string getFilename(const std::string& path)
	{
		size_t pos = path.find_last_of("/\\");
		return (pos != std::string::npos) ? path.substr(pos + 1) : path;
	}

	std::string getFilenameRaw(const std::string& path)
	{
		size_t pos = path.find_last_of("/\\");
		std::string filename = (pos != std::string::npos) ? path.substr(pos + 1) : path;
		size_t dotPos = filename.find_last_of('.');
		return (dotPos != std::string::npos) ? filename.substr(0, dotPos) : filename;
	}

	std::string getFileExtension(const std::string& filename) {
		// Find last period in filename
		size_t pos = filename.find_last_of(".");

		// Period found and period is not at beginning or end of filename
		if (pos != std::string::npos && pos != 0) {
			// Extract the extension
			return filename.substr(pos);
		}

		// Return empty string if no extension was found
		return "";
	}

	std::vector<std::string> getFilesWithExtensions(const std::string& path, const std::vector<std::string>& extensions)
	{
		std::vector<std::string> files;

		// Check if the path exists and is a directory
		if (!std::filesystem::exists(path))
		{
			Console::out::warning("IO", "Path does not exist: '" + path + "'");
			return files;
		}

		if (!std::filesystem::is_directory(path))
		{
			Console::out::warning("IO", "Error: Path is not a directory: '" + path + "'");
			return files;
		}

		// Iterate through the directory and subdirectories
		for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
		{
			if (std::filesystem::is_regular_file(entry)) // Only consider files
			{
				std::string filePath = entry.path().string();

				// Get file extension and convert to lowercase for case-insensitive matching
				std::string ext = entry.path().extension().string();
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
		}

		return files;
	}

}