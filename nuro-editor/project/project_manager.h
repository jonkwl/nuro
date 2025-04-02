#pragma once

#include <string>
#include <nlohmann/json.hpp>

#include <filesystem>
namespace fs = std::filesystem;

#include "../project/project_observer.h"
#include "../project/project_assets.h"

struct Project {
	// Root path of the project
	fs::path path;

	// Get relative path to project path
	fs::path pathRelative(fs::path other) const {
		return fs::relative(path, other);
	}

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
	bool load(const fs::path& directory);

	// Returns the currently loaded project
	const Project& project() const;

	// Returns the project observer
	ProjectObserver& observer();

	// Returns the project assets
	ProjectAssets& assets();

private:
	// Ensures configuration of the current project exists, returns false if creating failed
	bool ensureConfig();

	Project _project;
	ProjectObserver _observer;
	ProjectAssets _assets;
};