#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "../src/rendering/shader/shader.h"
#include "../src/utils/log.h"
#include "../src/utils/iohandler.h"

class ShaderBuilder
{
public:
	static void loadAndCompile(std::vector<std::string> path);
	static Shader* get(std::string name);
private:
	static std::vector<Shader*> shaders;
	static std::vector <std::string> identifiers;
};

