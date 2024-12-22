#include "components.h"

TransformComponent::TransformComponent() : position(glm::vec3(0.0f)),
rotation(glm::quat()),
scale(glm::vec3(1.0f)),
model(glm::mat4(1.0f)),
mvp(glm::mat4(1.0f))
{
}

Mesh nullMesh;
MeshRendererComponent::MeshRendererComponent() : mesh(nullMesh)
{
}

MeshRendererComponent::MeshRendererComponent(Mesh& mesh) : mesh(mesh)
{
}

CameraComponent::CameraComponent() : fov(70.0f),
near(0.3f),
far(1000.0f)
{
}

VelocityComponent::VelocityComponent() : intensity(1.0f)
{
}
