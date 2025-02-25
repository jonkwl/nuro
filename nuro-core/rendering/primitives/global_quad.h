#pragma once

#include <cstdint>

namespace GlobalQuad
{

	void create();
	void bind();
	void render();

	const uint32_t getVBO();
	const uint32_t getVAO();

};
