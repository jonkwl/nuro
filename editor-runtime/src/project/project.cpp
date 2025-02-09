#include "project.h"

#include "../core/utils/console.h"

Project::Project() : _configuration(),
_path()
{
}

void Project::load(const fs::path& directory)
{
	if (!fs::exists(directory) || !fs::is_directory(directory)) {
		Console::out::warning("Project", "Project path is invalid.");
	}

	_path = directory;

	if (!configValid()) {
		Console::out::warning("Project", "Project configuration could not be found!", "Make sure .project file exists in the project root folder.");
	}
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

bool Project::configValid()
{
	fs::path file_path = fs::path(_path) / ".project";
	return fs::exists(file_path) && fs::is_regular_file(file_path);
}
