#pragma once

#include <vector>
#include <glm.hpp>

#include "../core/viewport/viewport.h"
#include "../core/rendering/gizmos/quick_gizmo.h"

class Entity;
class Skybox;

class ForwardPass
{
public:
	ForwardPass(const Viewport& viewport);

	void create(const unsigned int msaaSamples); // Creates forward pass
	void destroy(); // Destroys forward pass

	unsigned int render(std::vector<Entity*>& targets); // Forward passes all entity render targets and returns color output

	unsigned int getDepthOutput(); // Returns depth output

	void enableSkybox(Skybox* source);
	void disableSkybox();

	void enableQuickGizmo(QuickGizmo* source);
	void disableQuickGizmo();

	bool wireframe; // Wireframe rendering mode
	glm::vec4 clearColor; // Clear color for forward pass
private:
	const Viewport& viewport; // Viewport forward pass instance is linked to

	Skybox* skybox; // Skybox that will be rendered during forward pass (optional)
	QuickGizmo* quickGizmo; // Quick gizmo instance that will be rendered during forward pass (optional)

	unsigned int outputFbo;	 // Output framebuffer
	unsigned int outputColor; // Output texture
	unsigned int outputDepth; // Output texture

	unsigned int multisampledFbo;		 // Anti-Aliasing framebuffer
	unsigned int multisampledRbo;		 // Anti-Aliasing renderbuffer
	unsigned int multisampledColorBuffer; // Anti-Aliasing color buffer texture
};