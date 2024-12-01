#pragma once

#include <glm.hpp>

namespace Input
{

	void setupInputs();
	void updateInputs();

	glm::vec2 getKeyAxis();
	glm::vec2 getMouseAxis();

};