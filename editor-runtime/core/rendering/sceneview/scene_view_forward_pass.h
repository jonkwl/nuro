#pragma once

#include <vector>
#include <glm.hpp>

#include "../core/viewport/viewport.h"
#include "../core/rendering/gizmos/imgizmo.h"
#include "../core/rendering/material/imaterial.h"
#include "../core/rendering/material/unlit/unlit_material.h"
#include "../core/rendering/material/lit/lit_material.h"

class OldEntity;
class Skybox;

class SceneViewForwardPass
{
public:
	explicit SceneViewForwardPass(const Viewport& viewport);

	void create(unsigned int msaaSamples); // Creates forward pass
	void destroy(); // Destroys forward pass

	// Scene view forward passes all entity render targets and returns color output
	unsigned int render(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& viewProjection);

	void setSkybox(Skybox* skybox);
	bool drawSkybox; // Draw skybox in scene view

	void setQuickGizmo(IMGizmo* source);
	bool drawQuickGizmos;

	bool wireframe; // Wireframe rendering mode
	glm::vec4 clearColor; // Clear color for forward pass
private:
	const Viewport& viewport; // Viewport forward pass instance is linked to

	Skybox* skybox; // Skybox that will be rendered during forward pass (optional)
	IMGizmo* imGizmo; // Quick gizmo instance that will be rendered during forward pass (optional)

	unsigned int outputFbo;	 // Output framebuffer
	unsigned int outputColor; // Output color

	unsigned int multisampledFbo; // Anti-aliasing framebuffer
	unsigned int multisampledRbo; // Anti-aliasing renderbuffer
	unsigned int multisampledColorBuffer; // Anti-aliasing colorbuffer

	UnlitMaterial* selectionMaterial; // Material for selection outline
	LitMaterial* defaultMaterial; // Some default material

	void renderSelectedEntity(OldEntity* selected); // Renders the selected entity with an outline
};