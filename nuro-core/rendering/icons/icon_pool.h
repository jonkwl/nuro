#pragma once

#include <string>

#include <utils/fsutil.h>

class Texture;

namespace IconPool {
	
	// Loads all icons from the given directory synchronously
	void loadAllSync(const path& directory);

	// Loads all icons from the given directory asynchronously
	void loadAllAsync(const path& directory);

	// Returns the texture backend id of an icon by the given identifier
	uint32_t get(const std::string& identifier);

	// Creates a fallback icon for invalid icons synchronously
	void createFallbackIcon(const path& path);

}