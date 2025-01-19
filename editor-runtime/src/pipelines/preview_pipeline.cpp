#include "preview_pipeline.h"

#include <glad/glad.h>

#include "../core/rendering/model/model.h"
#include "../core/rendering/shader/shader.h"
#include "../core/rendering/shader/shader_pool.h"
#include "../core/rendering/primitives/global_quad.h"

PreviewPipeline::PreviewPipeline() : viewport(),
fbo(0),
outputs()
{
}

void PreviewPipeline::create(float width, float height)
{
	// Adjust viewport size
	viewport.resize(width, height);

	// Generate preview renderer framebuffer
	glCreateFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void PreviewPipeline::destroy()
{
	// Delete framebuffer
	glDeleteFramebuffers(1, &fbo);
	fbo = 0;

	// Delete all outputs
	for (uint32_t output : outputs) {
		glDeleteTextures(1, &output);
	}
	outputs.clear();
}

size_t PreviewPipeline::createOutput()
{
	// Generate new output texture
	uint32_t renderTarget = 0;
	glGenTextures(1, &renderTarget);
	glBindTexture(GL_TEXTURE_2D, renderTarget);

	// Setup output texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, viewport.getWidth_gl(), viewport.getHeight_gl(), 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Add output to outputs and return index
	outputs.push_back(renderTarget);
	return outputs.size() - 1;
}

uint32_t PreviewPipeline::getOutput(size_t index) const
{
	if (index >= outputs.size()) return 0;
	return outputs[index];
}

void PreviewPipeline::resize(float width, float height)
{
	// Resize viewport
	viewport.resize(width, height);

	// Resize all output textures
	for (uint32_t output : outputs) {
		// Bind the texture
		glBindTexture(GL_TEXTURE_2D, output);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, viewport.getWidth_gl(), viewport.getHeight_gl(), 0, GL_RGBA, GL_FLOAT, NULL);
	}
}

void PreviewPipeline::render(uint32_t targetIndex, Model* model)
{
	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Attach output texture to framebuffer
	uint32_t output = outputs[targetIndex];
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output, 0);

	// Clear framebuffer
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Set viewport
	glViewport(0, 0, viewport.getWidth_gl(), viewport.getHeight_gl());

	Shader* testShader = ShaderPool::get("debug_pass");
	testShader->bind();

	for (const Mesh& mesh : model->getMeshes()) {
		// Bind and render mesh
		glBindVertexArray(mesh.getVAO());
		glDrawElements(GL_TRIANGLES, mesh.getIndiceCount(), GL_UNSIGNED_INT, 0);
	}
}