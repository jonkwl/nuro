#include "ambient_occlusion_pass.h"

#include "../src/rendering/shader/shader_pool.h"
#include "../src/rendering/core/mesh_renderer.h"
#include "../src/rendering/primitives/quad.h"
#include "../src/window/window.h"
#include "../src/utils/log.h"

unsigned int AmbientOcclusionPass::fbo = 0;

unsigned int AmbientOcclusionPass::aoOutput = 0;
unsigned int AmbientOcclusionPass::compositeOutput = 0;

Shader* AmbientOcclusionPass::aoPassShader = nullptr;
Shader* AmbientOcclusionPass::compositeShader = nullptr;

void AmbientOcclusionPass::setup()
{
	// Get shaders
	aoPassShader = ShaderPool::get("ambient_occlusion_pass");

	// Set ambient occlusion pass shaders static uniforms
	aoPassShader->bind();
	aoPassShader->setInt("depthInput", AMBIENT_OCCLUSION_DEPTH_UNIT);
	aoPassShader->setInt("normalInput", AMBIENT_OCCLUSION_NORMAL_UNIT);

	// Generate framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Generate ambient occlusion output texture
	glGenTextures(1, &aoOutput);
	glBindTexture(GL_TEXTURE_2D, aoOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::width, Window::height, 0, GL_RGBA, GL_FLOAT, nullptr);

	// Set ambient occlusion output texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Attach ambient occlusion output texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, aoOutput, 0);

	// Generate composite output texture
	glGenTextures(1, &compositeOutput);
	glBindTexture(GL_TEXTURE_2D, compositeOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::width, Window::height, 0, GL_RGBA, GL_FLOAT, nullptr);

	// Set composite output texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Check framebuffer status
	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		Log::printError("Framebuffer", "Error generating fbo: " + std::to_string(fboStatus));
	}

	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int AmbientOcclusionPass::render(unsigned int hdrInput, unsigned int depthInput, unsigned int normalInput)
{
	// Perform ambient occlusion pass
	ambientOcclusionPass(depthInput, normalInput);

	// Perform composite pass: Apply ambient occlusion from previous pass to hdr input
	compositePass(hdrInput);

	// Return output
	return compositeOutput;
}

void AmbientOcclusionPass::ambientOcclusionPass(unsigned int depthInput, unsigned int normalInput)
{
	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Bind ambient occlusion pass shader
	aoPassShader->bind();

	// Set ambient occlusion pass shader uniforms
	aoPassShader->setVec2("depthResolution", glm::vec2(Window::width, Window::height));
	aoPassShader->setMatrix4("inverseViewProjectionMatrix", glm::inverse(MeshRenderer::currentViewProjectionMatrix));

	// Bind depth input
	glActiveTexture(GL_TEXTURE0 + AMBIENT_OCCLUSION_DEPTH_UNIT);
	glBindTexture(GL_TEXTURE_2D, depthInput);

	// Bind normal input
	glActiveTexture(GL_TEXTURE0 + AMBIENT_OCCLUSION_NORMAL_UNIT);
	glBindTexture(GL_TEXTURE_2D, normalInput);

	// Bind and render to quad
	Quad::bind();
	Quad::render();
}

void AmbientOcclusionPass::compositePass(unsigned int hdrInput)
{
	compositeOutput = aoOutput;
}
