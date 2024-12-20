#pragma once

#include <vector>
#include <glm.hpp>

#include "../core/viewport/viewport.h"
#include "../core/rendering/gizmos/imgizmo.h"
#include "../core/rendering/material/lit/lit_material.h"

class Skybox;

class ForwardPass
{
public:
	explicit ForwardPass(const Viewport& viewport);

	void create(const unsigned int msaaSamples); // Creates forward pass
	void destroy(); // Destroys forward pass

	// Forward passes all entity render targets and returns color output
	unsigned int render(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& viewProjection);

	unsigned int getDepthOutput(); // Returns depth output

	void setSkybox(Skybox* source);
	void disableSkybox();

	void enableQuickGizmo(IMGizmo* source);
	void disableQuickGizmo();

	void setClearColor(glm::vec4 clearColor); // Clear color for forward pass
private:
	const Viewport& viewport; // Viewport forward pass instance is linked to

	Skybox* skybox; // Skybox that will be rendered during forward pass (optional)
	IMGizmo* imGizmo; // Quick gizmo instance that will be rendered during forward pass (optional)
	glm::vec4 clearColor; // Clear color for forward pass

	unsigned int outputFbo;	 // Output framebuffer
	unsigned int outputColor; // Output texture
	unsigned int outputDepth; // Output texture

	unsigned int multisampledFbo;		 // Anti-aliasing framebuffer
	unsigned int multisampledRbo;		 // Anti-aliasing renderbuffer
	unsigned int multisampledColorBuffer; // Anti-aliasing color buffer texture

	LitMaterial* defaultMaterial; // Some default material
};