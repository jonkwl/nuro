#include "post_processing.h"

PostProcessingSetup PostProcessing::setup = PostProcessingSetup();
unsigned int PostProcessing::fbo = 0;
unsigned int PostProcessing::rbo = 0;
unsigned int PostProcessing::screenTexture = 0;
unsigned int PostProcessing::vao = 0;
unsigned int PostProcessing::vbo = 0;
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

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	// Create framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Create framebuffer texture
	glGenTextures(1, &screenTexture);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::width, Window::height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);

	// Create render buffer
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Window::width, Window::height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// Check for framebuffer error
	GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		Log::printError("Framebuffer", "Error generating framebuffer: " + std::to_string(fboStatus));
	}
}

void PostProcessing::bind()
{
	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PostProcessing::render()
{
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
	glBindVertexArray(vao);
	glDisable(GL_DEPTH_TEST);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}
