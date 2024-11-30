#pragma once

#include <string>
#include <vector>

class Shader;

class ShaderPool
{
public:
	static void loadAndCompile(std::vector<std::string> path);
	static Shader* get(std::string name);

private:
	static std::vector<Shader*> shaders;
	static std::vector<std::string> identifiers;
};
