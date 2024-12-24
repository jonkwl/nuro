#pragma once

#include <cstdint>
#include <vector>
#include <glm.hpp>

#include "../core/viewport/viewport.h"
#include "../core/ecs/ecs_collection.h"
#include "../core/rendering/gizmos/imgizmo.h"
#include "../core/rendering/material/imaterial.h"
#include "../core/rendering/material/imaterial.h"
#include "../core/rendering/material/unlit/unlit_material.h"
#include "../core/rendering/material/lit/lit_material.h"

class Skybox;

class SceneViewForwardPass
{
public:
	explicit SceneViewForwardPass(const Viewport& viewport);

	void create(uint32_t msaaSamples); // Creates forward pass
	void destroy(); // Destroys forward pass

	// Scene view forward passes all entity render targets and returns color output
	uint32_t render(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& viewProjection, uint16_t nSelected, entt::entity selected);

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
	LitMaterial* defaultMaterial; // Some default material

	void renderMesh(TransformComponent& transform, MeshRendererComponent& renderer, IMaterial* material); // Renders a given entities mesh

	void renderSelectedEntity(entt::entity entity, const glm::mat4& viewProjection); // Renders the selected entity with an outline
};