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

	void setSkybox(Skybox* source);
	void disableSkybox();

	void enableQuickGizmo(QuickGizmo* source);
	void disableQuickGizmo();

	void setClearColor(glm::vec4 clearColor); // Clear color for forward pass
private:
	const Viewport& viewport; // Viewport forward pass instance is linked to

	Skybox* skybox; // Skybox that will be rendered during forward pass (optional)
	QuickGizmo* quickGizmo; // Quick gizmo instance that will be rendered during forward pass (optional)
	glm::vec4 clearColor; // Clear color for forward pass

	unsigned int outputFbo;	 // Output framebuffer
	unsigned int outputColor; // Output texture
	unsigned int outputDepth; // Output texture

	unsigned int multisampledFbo;		 // Anti-aliasing framebuffer
	unsigned int multisampledRbo;		 // Anti-aliasing renderbuffer
	unsigned int multisampledColorBuffer; // Anti-aliasing color buffer texture
};