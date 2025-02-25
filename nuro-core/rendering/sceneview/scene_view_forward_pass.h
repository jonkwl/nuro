#pragma once

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

#include <viewport/viewport.h>
#include <ecs/ecs_collection.h>
#include <rendering/gizmos/imgizmo.h>

class Skybox;
class IMaterial;
class UnlitMaterial;

class SceneViewForwardPass
{
public:
	explicit SceneViewForwardPass(const Viewport& viewport);

	void create(uint32_t msaaSamples); // Creates forward pass
	void destroy(); // Destroys forward pass

	// Scene view forward passes all entity render targets and returns color output
	uint32_t render(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& viewProjection, const Camera& camera, const std::vector<EntityContainer*>& selectedEntities);

	void linkSkybox(Skybox* skybox);
	bool drawSkybox; // Draw skybox in scene view

	void linkGizmos(IMGizmo* gizmos);
	bool drawGizmos; // Draw gizmos in scene view

	bool wireframe; // Wireframe rendering mode
	glm::vec4 clearColor; // Clear color for forward pass
private:
	const Viewport& viewport; // Viewport forward pass instance is linked to

	Skybox* skybox; // Skybox that will be rendered during forward pass (optional)
	IMGizmo* gizmos; // Gizmo instance that will be rendered during forward pass (optional)

	uint32_t outputFbo;	 // Output framebuffer
	uint32_t outputColor; // Output color

	uint32_t multisampledFbo; // Anti-aliasing framebuffer
	uint32_t multisampledRbo; // Anti-aliasing renderbuffer
	uint32_t multisampledColorBuffer; // Anti-aliasing colorbuffer

	UnlitMaterial* selectionMaterial; // Material for selection outline

	// Default scene view clearing color rgb values
	static constexpr float defaultClearColor[3] = { 0.015f, 0.015f, 0.015f };

	void renderMesh(TransformComponent& transform, MeshRendererComponent& renderer); // Renders a given entities mesh
	void renderMeshes(const std::vector<EntityContainer*>& skippedEntities); // Renders all meshes
	void renderSelectedEntity(EntityContainer* entity, const glm::mat4& viewProjection, const Camera& camera); // Renders the selected entity with an outline
};