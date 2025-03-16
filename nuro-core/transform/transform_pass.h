#pragma once

#include <vector>
#include <glm/glm.hpp>

#include <transform/transform.h>

class TransformPass
{
public:
	// Performs the preprocessing needed before rendering for all entities
	void perform(glm::mat4 viewProjection);

private:
	void evaluate(TransformComponent& transform);
	void evaluate(TransformComponent& transform, TransformComponent& parent, bool propagateModified);
};