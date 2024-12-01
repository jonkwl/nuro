#pragma once

#include <glm.hpp>

namespace Input
{

	void setup(); // Setup input system
	void step(); // Fetch inputs of current frame

	const glm::vec2& getKeyAxis();
	const glm::vec2& getMouseAxis();
	const glm::vec2& getScrollAxis();

};