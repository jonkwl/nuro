#include "transform_pass.h"

#include <algorithm>

#include <ecs/ecs_collection.h>
#include <transform/transform.h>
#include <diagnostics/profiler.h>

void TransformPass::perform(glm::mat4 viewProjection)
{
	// Evaluate transforms
	for (auto [entity, transform] : ECS::main().view<TransformComponent>().each()) {
		// Evaluate root node
		if (Transform::isRoot(transform)) evaluate(transform);

		// Update transforms model-view-projection
		Transform::updateMvp(transform, viewProjection);
	}
}

void TransformPass::evaluate(TransformComponent& transform)
{
	if(transform.modified) Transform::evaluate(transform);

	for (Entity child : transform.children) {
		evaluate(ECS::main().get<TransformComponent>(child), transform, transform.modified);
	}

	transform.modified = false;
}

void TransformPass::evaluate(TransformComponent& transform, TransformComponent& parent, bool propagateModified)
{
	if (propagateModified) transform.modified = true;

	if(transform.modified) Transform::evaluate(transform, parent);

	for (Entity child : transform.children) {
		evaluate(ECS::main().get<TransformComponent>(child), transform, transform.modified);
	}

	transform.modified = false; 
}