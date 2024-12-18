#include "components.h"

TransformComponent::TransformComponent() : position(glm::vec3(0.0f)),
rotation(glm::quat()),
scale(glm::vec3(1.0f)),
modelMatrix(glm::mat4(1.0f)),
mvpMatrix(glm::mat4(1.0f)),
normalMatrix(glm::mat4(1.0f))
{
}

MeshRendererComponent::MeshRendererComponent() : mesh(nullptr)
{
}

CameraComponent::CameraComponent() : fov(70.0f),
near(0.3f),
far(1000.0f)
{
}