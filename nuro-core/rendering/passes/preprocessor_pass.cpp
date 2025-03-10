#include "preprocessor_pass.h"

#include <algorithm>

#include <diagnostics/profiler.h>
#include <ecs/ecs_collection.h>
#include <transform/transform.h>

void PreprocessorPass::perform(glm::mat4 viewProjection)
{
	auto transforms = ECS::gRegistry.view<TransformComponent>();

	// Evaluate transforms
	for (auto [entity, transform] : transforms.each()) {
		// Initiate recursive evaluation for root transforms
		if (!Transform::hasParent(transform)) {
			Transform::evaluateRecursive(transform);
		}

		// Evaluate transforms model-view-projection
		Transform::evaluateMvp(transform, viewProjection);
	}

	// Create mvp matrix and reset modification state
	for (auto [entity, transform] : transforms.each()) {
		transform.modified = false;
	}
}