#include "post_processing.h"

PostProcessingSetup PostProcessing::setup = PostProcessingSetup();

unsigned int PostProcessing::screenTexture = 0;

unsigned int PostProcessing::ppFbo = 0;
unsigned int PostProcessing::ppRbo = 0;

unsigned int PostProcessing::msaaFbo = 0;
unsigned int PostProcessing::msaaRbo = 0;
unsigned int PostProcessing::msaaColorBuffer = 0;
unsigned int PostProcessing::msaaSampleAmount = 4;

unsigned int PostProcessing::renderVao = 0;
unsigned int PostProcessing::renderVbo = 0;

Shader* PostProcessing::shader = nullptr;

void PostProcessing::initialize()
{
	// Get default post processing shader
	shader = ShaderBuilder::get("framebuffer");
	 
	// Create vao for scren render quad
	float vertices[] =
	{
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,

		 1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f
	};

	glGenVertexArrays(1, &renderVao);
	glGenBuffers(1, &renderVbo);
	glBindVertexArray(renderVao);
	glBindBuffer(GL_ARRAY_BUFFER, renderVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	// Create post processing framebuffer
	glGenFramebuffers(1, &ppFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, ppFbo);

	// Create post processing framebuffer texture
	glGenTextures(1, &screenTexture);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::width, Window::height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);

	// Create post processing render buffer
	glGenRenderbuffers(1, &ppRbo);
	glBindRenderbuffer(GL_RENDERBUFFER, ppRbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Window::width, Window::height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ppRbo);

	// Check for post processing framebuffer error
	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		Log::printError("Framebuffer", "Error generating framebuffer: " + std::to_string(fboStatus));
	}

	// Create anti-aliasing framebuffer
	glGenFramebuffers(1, &msaaFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, msaaFbo);

	// Create multi-sampled color buffer texture
	glGenTextures(1, &msaaColorBuffer);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msaaColorBuffer);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaaSampleAmount, GL_RGBA16F, Window::width, Window::height, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, msaaColorBuffer, 0);

	// Create multi-sampled renderbuffer for depth and stencil
	glGenRenderbuffers(1, &msaaRbo);
	glBindRenderbuffer(GL_RENDERBUFFER, msaaRbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSampleAmount, GL_DEPTH24_STENCIL8, Window::width, Window::height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msaaRbo);

	// Check for multi-sampled framebuffer error
	fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		Log::printError("Framebuffer", "Error generating framebuffer: " + std::to_string(fboStatus));
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessing::bind()
{
	// Bind framebuffer
	// glBindFramebuffer(GL_FRAMEBUFFER, ppFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, msaaFbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PostProcessing::render()
{
	// Bilt multi-sampled framebuffer to post processing framebuffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, msaaFbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ppFbo);
	glBlitFramebuffer(0, 0, Window::width, Window::height, 0, 0, Window::width, Window::height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screenTexture);

	// Bind shader and set uniforms
	shader->bind();

	shader->setInt("screenTexture", 0);
	shader->setVec2("screenResolution", glm::vec2(Window::width, Window::height));

	shader->setFloat("exposure", setup.exposure);
	shader->setFloat("contrast", setup.contrast);
	shader->setFloat("gamma", setup.gamma);

	shader->setBool("chromaticAberration", setup.chromaticAberration);
	shader->setFloat("chromaticAberrationStrength", setup.chromaticAberrationStrength);
	shader->setFloat("chromaticAberrationRange", setup.chromaticAberrationRange);
	shader->setFloat("chromaticAberrationRedOffset", setup.chromaticAberrationRedOffset);
	shader->setFloat("chromaticAberrationBlueOffset", setup.chromaticAberrationBlueOffset);

	shader->setBool("vignette", setup.vignette);
	shader->setFloat("vignetteStrength", setup.vignetteStrength);
	shader->setVec3("vignetteColor", setup.vignetteColor);
	shader->setFloat("vignetteRadius", setup.vignetteRadius);
	shader->setFloat("vignetteSoftness", setup.vignetteSoftness);
	shader->setFloat("vignetteRoundness", setup.vignetteRoundness);

	// Bind vao and texture and render framebuffer to screen
	glBindVertexArray(renderVao);
	glDisable(GL_DEPTH_TEST);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glEnable(GL_DEPTH_TEST);
}
