#pragma once

#include <glm.hpp>
#include <vector>

#include "../core/viewport/viewport.h"

class Model;

class PreviewPipeline
{
public:
	PreviewPipeline();

	// Creates the preview renderer frame
	void create(float width, float height);

	// Destroys the preview renderer and all of the output textures connected with it
	void destroy();

	// Creates a new output render texture and returns its index
	size_t createOutput();

	// Returns output render texture by given index 
	uint32_t getOutput(size_t index) const;

private:
	Viewport viewport;
	uint32_t fbo;
	std::vector<uint32_t> outputs;

private:
	void resize(float width, float height);
	void render(uint32_t outputTargetIndex, Model* model);
};