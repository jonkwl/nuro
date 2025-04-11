#pragma once

#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>

#include "../project/project_observer.h"
#include "../project/project_assets.h"

struct Project {
	// Root path of the project
	std::filesystem::path path;

	// Get relative path to project path
	std::filesystem::path pathRelative(const std::filesystem::path& other) const {
		return std::filesystem::relative(path, other);
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
	bool load(const std::filesystem::path& directory);

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