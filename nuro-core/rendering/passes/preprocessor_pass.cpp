#include "preprocessor_pass.h"

#include <algorithm>

#include <ecs/ecs_collection.h>
#include <transform/transform.h>

void PreprocessorPass::perform(glm::mat4 viewProjection)
{
	uint32_t modified = 0;

	// Evaluate transforms
	for (TransformComponent* transform : ECS::getTransformQueue()) {
		if (Transform::evaluate(*transform)) modified++;
		Transform::createMvp(*transform, viewProjection);
	}

	// Reset transforms modification state
	for (TransformComponent* transform : ECS::getTransformQueue()) {
		transform->modified = false;
	}
}