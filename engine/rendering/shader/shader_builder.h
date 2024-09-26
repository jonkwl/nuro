#pragma once

#include <string>
#include <vector>

#include "../engine/rendering/shader/shader.h"
#include "../engine/utils/log/log.h"
#include "../engine/utils/iohandler/iohandler.h"

class ShaderBuilder
{
public:
	static std::vector<Shader> loadAndCompile(std::vector<std::string> path);
};

