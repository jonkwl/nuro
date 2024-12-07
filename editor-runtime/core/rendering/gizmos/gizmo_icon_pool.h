#pragma once

#include <string>

#include "../core/rendering/texture/texture.h"

namespace GizmoIconPool {
	
	void loadAll(std::string directoryPath);

	Texture& get(std::string identifier);

}