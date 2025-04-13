#include "project_manager.h"

#include <fstream>

#include <utils/console.h>

ProjectManager::ProjectManager() : _project(),
_observer(),
_assets() 
{
}

void ProjectManager::pollEvents()
{
	_observer.pollEvents();
}

bool ProjectManager::load(const FS::Path& directory)
{
	if (!FS::exists(directory) || !FS::isDirectory(directory)) {
		Console::out::error("Project", "Project path is invalid.", "Provided path was '" + directory.string() + "'");
		return false;
	}

	// Set project path
	_project.path = directory;

	// Ensure project configuration is valid
	if (!ensureConfig()) 
		return false;

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

ProjectAssets& ProjectManager::assets()
{
	return _assets;
}

bool ProjectManager::ensureConfig()
{
	// Default configuration path
	FS::Path configPath = FS::Path(_project.path) / ".project";

	// Touch configuration file
	if (FS::touch(configPath)) 
		return true;

	Console::out::error("Project", "Couldn't locate or create project configuration.", "Tried to create it at '" + configPath.string() + "'");
	return false;
}