#pragma once

#include <string>

class Texture;

namespace IconPool {

	// Creates a fallback icon for invalid icons
	void createFallbackIconSync(const std::string& invalidIconPath);
	
	// Loads all icons from the given directory synchronously
	void loadAllSync(const std::string& directory);

	// Loads all icons from the given directory asynchronously
	void loadAllAsync(const std::string& directory);

	// Returns the texture backend id of an icon by the given identifier
	uint32_t get(const std::string& identifier);

}