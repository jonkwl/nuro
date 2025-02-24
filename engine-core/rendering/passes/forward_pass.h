#pragma once

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>

#include <ecs/components.h>
#include <viewport/viewport.h>
#include <rendering/gizmos/imgizmo.h>

class Skybox;

class ForwardPass
{
public:
	explicit ForwardPass(const Viewport& viewport);

	void create(const uint32_t msaaSamples); // Creates forward pass
	void destroy(); // Destroys forward pass

	// Forward passes all entity render targets and returns color output
	uint32_t render(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& viewProjection);

	uint32_t getDepthOutput(); // Returns depth output

	void linkSkybox(Skybox* source);
	bool drawSkybox;

	void linkGizmos(IMGizmo* gizmos);
	bool drawGizmos;

	void setClearColor(glm::vec4 clearColor); // Clear color for forward pass
private:
	const Viewport& viewport; // Viewport forward pass instance is linked to

	Skybox* skybox; // Skybox that will be rendered during forward pass (optional)
	IMGizmo* gizmos; // Gizmo instance that will be rendered during forward pass (optional)
	
	glm::vec4 clearColor; // Clear color for forward pass

	uint32_t outputFbo;	 // Output framebuffer
	uint32_t outputColor; // Output texture
	uint32_t outputDepth; // Output texture

	uint32_t multisampledFbo;		 // Anti-aliasing framebuffer
	uint32_t multisampledRbo;		 // Anti-aliasing renderbuffer
	uint32_t multisampledColorBuffer; // Anti-aliasing color buffer texture

	void renderMesh(TransformComponent& transform, MeshRendererComponent& renderer);
	void renderMeshes();
};