#pragma once

#include <string>

class Texture;

namespace IconPool {
	
	// Loads all icons from the given directory synchronously
	void loadAllSync(const std::string& directory);

	// Loads all icons from the given directory asynchronously
	void loadAllAsync(const std::string& directory);

	// Returns the texture backend id of an icon by the given identifier
	uint32_t get(const std::string& identifier);

	// Creates a fallback icon for invalid icons synchronously
	void createFallbackIcon(const std::string& path);

}