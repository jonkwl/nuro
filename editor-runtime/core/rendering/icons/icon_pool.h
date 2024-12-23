#pragma once

#include <string>

#include "../core/rendering/texture/texture.h"

namespace IconPool {
	
	void loadAll(std::string directoryPath);

	Texture& get(std::string identifier);

}