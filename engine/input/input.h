#pragma once

#include <glm.hpp>
#include "../engine/runtime.h"
#include "../engine/context/context.h"

class Input
{
public:
	static void setupInputs();
	static void updateInputs();

	static glm::vec2 keyAxis;
	static glm::vec2 keyAxisSmooth;
	static float keyAxisSmoothingFactor;

	static glm::vec2 mouseAxis;
	static glm::vec2 mouseLast;

	static bool escPressed;
};