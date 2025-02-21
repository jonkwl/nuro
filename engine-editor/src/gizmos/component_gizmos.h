#pragma once

#include "../src/core/ecs/ecs_collection.h"
#include "../src/core/rendering/gizmos/gizmos.h"

namespace ComponentGizmos
{

	// Renders icon gizmos of all components in register
	void renderSceneViewIcons(IMGizmo& gizmos, TransformComponent& cameraTransform);

	// Renders gizmos of all components for specific entity
	void renderEntityGizmos(IMGizmo& gizmos, EntityContainer& entity);

};
