#pragma once

#include <glm/glm.hpp>

class PreprocessorPass
{
public:
	// Performs the preprocessing needed before rendering for all entities
	void perform(glm::mat4 viewProjection);
};
