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
		// Evaluate root node
		if (Transform::isRoot(transform)) evaluate(transform);

		// Update transforms model-view-projection
		Transform::updateMvp(transform, viewProjection);
	}
}

void PreprocessorPass::evaluate(TransformComponent& transform)
{
	if(transform.modified) Transform::evaluate(transform);

	for (Entity child : transform.children) {
		evaluate(ECS::gRegistry.get<TransformComponent>(child), transform, transform.modified);
	}

	transform.modified = false;
}

void PreprocessorPass::evaluate(TransformComponent& transform, TransformComponent& parent, bool propagateModified)
{
	if (propagateModified) transform.modified = true;

	if(transform.modified) Transform::evaluate(transform, parent);

	for (Entity child : transform.children) {
		evaluate(ECS::gRegistry.get<TransformComponent>(child), transform, transform.modified);
	}

	transform.modified = false; 
}