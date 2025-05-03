#pragma once

#include <string>
#include <nlohmann/json.hpp>

#include <utils/fsutil.h>

#include "../project/project_observer.h"
#include "../project/project_assets.h"

struct Project {
	// Root path of the project
	FS::Path path;

	// Configuration of the project
	struct Config {
		std::string name = "Unnamed Project*";
	} config;
};

class ProjectManager
{
public:
	ProjectManager();

	// Dispatches any project related events
	void pollEvents();

	// Loads the project located in the given directory
	bool load(const FS::Path& directory);

	// Returns the currently loaded project
	const Project& project() const;

	// Returns the project observer
	ProjectObserver& observer();

	// Returns the project assets
	ProjectAssets& assets();

	// Converts a relative path within the current project to an absolute path
	FS::Path abs(const FS::Path& path);

private:
	// Ensures configuration of the current project exists, returns false if creating failed
	bool ensureConfig();

	Project _project;
	ProjectObserver _observer;
	ProjectAssets _assets;
};