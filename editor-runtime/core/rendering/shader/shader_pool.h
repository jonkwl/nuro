#pragma once

#include <string>
#include <vector>

class Shader;

namespace ShaderPool
{
	// Loads and compiles all shaders within the given paths (each folder within a given path is treated as a shader and expected to contain the shader source files)
	void load(const std::vector<std::string>& path);

	// Returns a global empty default shader
	Shader* empty();

	// Returns a loaded shader by its name
	Shader* get(const std::string& name);
};
