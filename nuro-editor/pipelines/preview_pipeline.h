#pragma once

#include <glm/glm.hpp>
#include <vector>

#include <ecs/components.h>
#include <viewport/viewport.h>

class Model;
class LitMaterial;

struct PreviewOutput {
	Viewport viewport;
	uint32_t texture = 0;
	bool resizePending = false;
};

struct PreviewRenderInstruction {
	size_t outputIndex = 0;
	glm::vec4 backgroundColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	Model* model = nullptr;
	LitMaterial* modelMaterial = nullptr;
	TransformComponent modelTransform;
	TransformComponent cameraTransform;
};

class PreviewPipeline
{
public:
	PreviewPipeline();

	// Creates the preview renderer frame
	void create();

	// Destroys the preview renderer and all of the output textures connected with it
	void destroy();

	// Renders all render instructions queued
	void render();

	// Creates a new preview output and returns its index
	size_t createOutput();

	// Returns preview output by given index 
	const PreviewOutput& getOutput(size_t index) const;

	// Resizes an output by given index
	void resizeOutput(size_t index, float width, float height);

	// Adds the given render instruction to the rendering queue
	void addRenderInstruction(PreviewRenderInstruction instruction);

private:
	uint32_t fbo;
	uint32_t rbo;
	std::vector<PreviewOutput> outputs;
	std::vector<PreviewRenderInstruction> renderInstructions;
};