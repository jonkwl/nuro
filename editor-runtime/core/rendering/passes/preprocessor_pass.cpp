#include "preprocessor_pass.h"

#include "../core/ecs/ecs_collection.h"
#include "../core/transform/transform.h"

void PreprocessorPass::perform(glm::mat4 viewProjection)
{
	// Pre pass render each entity
	auto targets = ECS::gRegistry.view<TransformComponent, MeshRendererComponent>();
	for (auto [entity, transform, renderer] : targets.each()) {
		// Compute transform matrices
		Transform::evaluate(transform, viewProjection);
	}
}