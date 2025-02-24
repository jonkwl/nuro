#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <filesystem>

namespace fs = std::filesystem;

class Project
{
public:
	struct Configuration {
		std::string name = "Unnamed Project*";
	};

public:
	Project();

	// Loads the  project located in the given directory
	bool load(const fs::path& directory);

	// Returns the projects configuration as readonly
	const Configuration& config() const;

	// Returns the path of a project
	const fs::path& path() const;

	// Returns relative path to project path
	fs::path relative(const fs::path& path) const;

private:
	Configuration _configuration;
	fs::path _path;

	// Validates project configuration exists by creating one if it doesnt
	bool validateConfig();
};