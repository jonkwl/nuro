#pragma once

#include <vector>
#include <glm.hpp>

#include "../core/viewport/viewport.h"
#include "../core/rendering/gizmos/imgizmo.h"
#include "../core/rendering/material/imaterial.h"
#include "../core/rendering/material/unlit/unlit_material.h"

class Entity;
class Skybox;

class SceneViewForwardPass
{
public:
	SceneViewForwardPass(const Viewport& viewport);

	void create(unsigned int msaaSamples); // Creates forward pass
	void destroy(); // Destroys forward pass

	// Scene view forward passes all entity render targets and returns color output
	unsigned int render(std::vector<Entity*>& targets, Entity* selected);

	void setSkybox(Skybox* skybox);
	bool drawSkybox; // Draw skybox in scene view

	void setQuickGizmo(IMGizmo* source);
	bool drawQuickGizmos;

	bool wireframe; // Wireframe rendering mode
	glm::vec4 clearColor; // Clear color for forward pass
private:
	const Viewport& viewport; // Viewport forward pass instance is linked to

	Skybox* skybox; // Skybox that will be rendered during forward pass (optional)
	IMGizmo* quickGizmo; // Quick gizmo instance that will be rendered during forward pass (optional)

	unsigned int outputFbo;	 // Output framebuffer
	unsigned int outputColor; // Output color

	unsigned int multisampledFbo; // Anti-aliasing framebuffer
	unsigned int multisampledRbo; // Anti-aliasing renderbuffer
	unsigned int multisampledColorBuffer; // Anti-aliasing colorbuffer

	UnlitMaterial* selectionMaterial; // Material for selection outline

	void renderSelectedEntity(Entity* selected); // Renders the selected entity with an outline
};