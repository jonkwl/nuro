#pragma once

#include <string>

class Texture;

namespace IconPool {

	// Creates a fallback icon for invalid icons
	void createFallbackIconSync(const std::string& invalidIconPath);
	
	// Loads icons from the provided directory path synchronously
	void loadSync(const std::string& directoryPath);

	// Loads icons from the provided directory path asynchronously
	void loadAsync(const std::string& directoryPath);

	// Returns the texture source of an icon by the given identifier
	uint32_t get(const std::string& identifier);

}