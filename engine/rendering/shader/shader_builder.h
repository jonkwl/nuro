#pragma once

#include <string>
#include <vector>

#include "../engine/rendering/shader/shader.h"
#include "../engine/utils/log.h"
#include "../engine/utils/iohandler.h"

class ShaderBuilder
{
public:
	static std::vector<Shader*> loadAndCompile(std::vector<std::string> path);
};

