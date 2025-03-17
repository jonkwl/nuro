#include "preview_pipeline.h"

#include <glad/glad.h>

#include <utils/console.h>
#include <rendering/model/model.h>
#include <rendering/shader/shader.h>
#include <rendering/shader/shader_pool.h>
#include <rendering/primitives/global_quad.h>
#include <rendering/material/lit/lit_material.h>
#include <rendering/transformation/transformation.h>

PreviewPipeline::PreviewPipeline() : fbo(0),
outputs(),
renderInstructions()
{
}

void PreviewPipeline::create()
{
	// Generate framebuffer
	glCreateFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void PreviewPipeline::destroy()
{
	// Delete framebuffer
	glDeleteFramebuffers(1, &fbo);
	fbo = 0;

	// Delete all outputs
	for (PreviewOutput output : outputs) {
		glDeleteTextures(1, &output.texture);
	}
	outputs.clear();
}

size_t PreviewPipeline::createOutput()
{
	// Generate new output
	PreviewOutput output;
	output.viewport.resize(250.0f, 250.0f);

	// Generate outputs texture
	glGenTextures(1, &output.texture);
	glBindTexture(GL_TEXTURE_2D, output.texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, output.viewport.getWidth_gl(), output.viewport.getHeight_gl(), 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Add output to outputs and return index
	outputs.push_back(output);
	return outputs.size() - 1;
}

const PreviewOutput& PreviewPipeline::getOutput(size_t index) const
{
	// Securely fetch output by index
	if (index >= outputs.size()) return PreviewOutput();
	return outputs[index];
}

void PreviewPipeline::resizeOutput(size_t index, float width, float height)
{
	// Securely fetch output by index
	if (index >= outputs.size()) return;
	PreviewOutput& output = outputs[index];

	// Resize viewport and set to resized
	output.viewport.resize(width, height);
	output.resizePending = true;
}

void PreviewPipeline::addRenderInstruction(PreviewRenderInstruction instruction)
{
	renderInstructions.push_back(instruction);
}

void PreviewPipeline::render()
{
	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Perform all render instructions
	for (PreviewRenderInstruction instruction : renderInstructions) {
		
		// Return if nullpointers given
		if (!instruction.model || !instruction.modelMaterial) return;

		// Securely fetch output by index
		if (instruction.outputIndex >= outputs.size()) return;
		PreviewOutput& output = outputs[instruction.outputIndex];

		// Resize output texture if needed
		if (output.resizePending) {
			// Only resize if issued new size is big enough
			glBindTexture(GL_TEXTURE_2D, output.texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, output.viewport.getWidth_gl(), output.viewport.getHeight_gl(), 0, GL_RGBA, GL_FLOAT, NULL);
			output.resizePending = false;
		}

		// Attach output texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output.texture, 0);

		// Clear framebuffer
		glClearColor(instruction.backgroundColor.r, instruction.backgroundColor.g, instruction.backgroundColor.b, instruction.backgroundColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Set viewport
		glViewport(0, 0, output.viewport.getWidth_gl(), output.viewport.getHeight_gl());

		// Bind shader and material
		Shader* shader = instruction.modelMaterial->getShader();
		shader->bind();
		instruction.modelMaterial->bind();
		instruction.modelMaterial->setSampleDirectionalLight();

		// Calculate and sync transform matrices
		glm::mat4 _model = Transformation::model(instruction.modelTransform.position, instruction.modelTransform.rotation, instruction.modelTransform.scale);
		glm::mat4 _view = Transformation::view(instruction.cameraTransform.position, instruction.cameraTransform.rotation);
		glm::mat4 _projection = Transformation::projection(45.0f, output.viewport.getAspect(), 0.3f, 1000.0f);
		glm::mat4 _mvp = _projection * _view * _model;
		glm::mat4 _normal = Transformation::normal(_model);
		shader->setMatrix4("mvpMatrix", _mvp);
		shader->setMatrix4("modelMatrix", _model);
		shader->setMatrix3("normalMatrix", _normal);

		// Bind and render all meshes of model
		for (int i = 0; i < instruction.model->nLoadedMeshes(); i++) {
			const Mesh* mesh = instruction.model->queryMesh(i);
			glBindVertexArray(mesh->vao());
			glDrawElements(GL_TRIANGLES, mesh->indiceCount(), GL_UNSIGNED_INT, 0);
		}

		instruction.modelMaterial->syncLightUniforms();

	}

	// Clear render instructions
	renderInstructions.clear();

	// Bind screen framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}