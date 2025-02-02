#pragma once

#include <string>

#include "../core/rendering/texture/texture.h"

namespace IconPool {
	
	void load(const std::string& directoryPath);

	Texture& get(const std::string& identifier);

}