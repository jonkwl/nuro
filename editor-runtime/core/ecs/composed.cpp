#include "composed.h"

Camera::Camera(TransformComponent& transform, CameraComponent& root) : transform(transform),
root(root)
{
}
