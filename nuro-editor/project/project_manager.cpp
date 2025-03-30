
#include "project_manager.h"

#include <fstream>

#include <utils/console.h>

ProjectManager::ProjectManager() : _project(),
_observer()
{
}

void ProjectManager::pollEvents()
{
	_observer.pollEvents();
}

bool ProjectManager::load(const fs::path& directory)
{
	if (!fs::exists(directory) || !fs::is_directory(directory)) {
		Console::out::error("Project", "Project path is invalid.", "Provided path was '" + directory.string() + "'");
		return false;
	}

	// Set project path
	_project.path = directory;

	// Ensure project configuration is valid
	if (!ensureConfig()) return false;

	// Start observing project
	_observer.setTarget(_project.path);

	return true;
}

const Project& ProjectManager::project() const
{
	return _project;
}

ProjectObserver& ProjectManager::observer()
{
	return _observer;
}

bool ProjectManager::ensureConfig()
{
	// Default configuration path
	fs::path file_path = fs::path(_project.path) / ".project";

	// Check project configuration file exists
	if (fs::exists(file_path) && fs::is_regular_file(file_path)) return true;

	// Configuration doens't exist, create it
	std::ofstream file(file_path);
	if (file.is_open()) {
		Console::out::start("Project", "Created project configuration at '" + file_path.string() + "'");
		return true;
	}
	else {
		Console::out::error("Project", "Couldn't create project configuration.", "Tried to create it at '" + file_path.string() + "'");
		return false;
	}
}