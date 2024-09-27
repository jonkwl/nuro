#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "../engine/rendering/shader/shader.h"
#include "../engine/utils/log.h"
#include "../engine/utils/iohandler.h"

class ShaderBuilder
{
public:
	static void loadAndCompile(std::vector<std::string> path);
	static Shader* get(std::string name);
private:
	static std::vector<Shader*> shaders;
	static std::vector <std::string> identifiers;
};

