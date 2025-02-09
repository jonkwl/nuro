#pragma once

#include <string>
#include <vector>

class Shader;

namespace ShaderPool
{
	// Loads all shaders from the given directory synchronously
	void loadAllSync(const std::string& directory);

	// Loads all shaders from the given directory asynchronously
	void loadAllAsync(const std::string& directory);

	// Returns the global empty default shader
	Shader* empty();

	// Returns a loaded shader by the given identifier
	Shader* get(const std::string& identifier);
};
