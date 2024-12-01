#pragma once

#include <string>
#include <vector>

class Shader;

namespace ShaderPool
{
	void loadAndCompile(std::vector<std::string> path);
	Shader* get(std::string name);
};
