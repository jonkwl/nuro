#include "preprocessor_pass.h"

#include <algorithm>

#include <diagnostics/profiler.h>
#include <ecs/ecs_collection.h>
#include <transform/transform.h>

void PreprocessorPass::perform(glm::mat4 viewProjection)
{
	// Evaluate transforms
	for (TransformComponent* transform : ECS::getTransformQueue()) {
		Transform::evaluate(*transform);
		Transform::createMvp(*transform, viewProjection);
	}

	// Reset transforms modification state
	for (TransformComponent* transform : ECS::getTransformQueue()) {
		transform->modified = false;
	}
}