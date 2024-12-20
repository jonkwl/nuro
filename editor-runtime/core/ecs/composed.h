#pragma once

#include "../core/ecs/components.h"

// Collection of composed object structures from specific components for engine internal usage

struct Camera {

	explicit Camera(TransformComponent& transform, CameraComponent& root);

	TransformComponent& transform;
	CameraComponent& root;

};