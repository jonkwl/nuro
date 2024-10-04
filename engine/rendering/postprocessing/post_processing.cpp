#include "post_processing.h"

float PostProcessing::exposure = 1.0f;
float PostProcessing::contrast = 1.0f;
float PostProcessing::gamma = 2.2f;

bool PostProcessing::chromaticAberration = true;
float PostProcessing::chromaticAberrationStrength = 1.15f;
float PostProcessing::chromaticAberrationRange = 0.2f;
float PostProcessing::chromaticAberrationRedOffset = 0.01f;
float PostProcessing::chromaticAberrationBlueOffset = 0.01f;

bool PostProcessing::vignette = true;
glm::vec4 PostProcessing::vignetteColor = glm::vec4(0.0f, 0.0f, 0.0, 0.7f);
float PostProcessing::vignetteRadius = 0.66f;
float PostProcessing::vignetteSoftness = 0.36f;
float PostProcessing::vignetteRoundness = 1.35f;

unsigned int PostProcessing::fbo = 0;
unsigned int PostProcessing::rbo = 0;
unsigned int PostProcessing::texture = 0;
unsigned int PostProcessing::vao = 0;
unsigned int PostProcessing::vbo = 0;
Shader* PostProcessing::shader = nullptr;

void PostProcessing::Initialize()
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
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::width, Window::height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

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

void PostProcessing::Bind()
{
	// Bind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void PostProcessing::Render()
{
	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Bind shader and set uniforms
	shader->bind();
	shader->setVec2("screenResolution", glm::vec2(Window::width, Window::height));

	shader->setFloat("exposure", exposure);
	shader->setFloat("contrast", contrast);
	shader->setFloat("gamma", gamma);

	shader->setBool("chromaticAberration", chromaticAberration);
	shader->setFloat("chromaticAberrationStrength", chromaticAberrationStrength);
	shader->setFloat("chromaticAberrationRange", chromaticAberrationRange);
	shader->setFloat("chromaticAberrationRedOffset", chromaticAberrationRedOffset);
	shader->setFloat("chromaticAberrationBlueOffset", chromaticAberrationBlueOffset);

	shader->setBool("vignette", vignette);
	shader->setVec4("vignetteColor", vignetteColor);
	shader->setFloat("vignetteRadius", vignetteRadius);
	shader->setFloat("vignetteSoftness", vignetteSoftness);
	shader->setFloat("vignetteRoundness", vignetteRoundness);

	// Bind vao and texture and render framebuffer to screen
	glBindVertexArray(vao);
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
