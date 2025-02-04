#pragma once

#include <string>

class Texture;

namespace IconPool {

	// Creates a fallback icon for an invalid icon
	void createFallback(const std::string& invalidIconPath);
	
	// Loads icons from the provided directory path
	void load(const std::string& directoryPath);

	// Returns the texture source of an icon by the given identifier
	uint32_t get(const std::string& identifier);

}