#pragma once

#include <glm.hpp>

namespace Input
{

	void setupInputs();
	void updateInputs();

	const glm::vec2& getKeyAxis();
	const glm::vec2& getMouseAxis();

};