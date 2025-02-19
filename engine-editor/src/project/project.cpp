
#include "project.h"

#include <fstream>

#include "../src/core/utils/console.h"

Project::Project() : _configuration(),
_path()
{
}

bool Project::load(const fs::path& directory)
{
	if (!fs::exists(directory) || !fs::is_directory(directory)) {
		Console::out::error("Project", "Project path is invalid.", "Provided path was '" + directory.string() + "'");
		return false;
	}

	_path = directory;

	if (!validateConfig()) return false;

	return true;
}

const Project::Configuration& Project::config() const
{
	return _configuration;
}

const fs::path& Project::path() const
{
	return _path;
}

fs::path Project::relative(const fs::path& path) const
{
	return fs::relative(path, _path);
}

bool Project::validateConfig()
{
	fs::path file_path = fs::path(_path) / ".project";

	// Check if project configuration file exists
	if (fs::exists(file_path) && fs::is_regular_file(file_path)) return true;

	std::ofstream file(file_path);
	if (file.is_open()) {
		Console::out::processStart("Project", "Created project configuration at '" + file_path.string() + "'");
		return true;
	}
	else {
		Console::out::error("Project", "Couldn't create project configuration.", "Tried to create it at '" + file_path.string() + "'");
		return false;
	}
}