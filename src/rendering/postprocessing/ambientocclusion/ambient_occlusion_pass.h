#pragma once

#include <glad/glad.h>

#include "../src/rendering/shader/Shader.h"

class AmbientOcclusionPass
{
public:
	static void setup(); // Setup ambient occlusion pass
	static unsigned int render(unsigned int hdrInput, unsigned int depthInput); // Render ambient occlusion pass and return output
private:
	static unsigned int fbo; // Framebuffer

	static unsigned int aoOutput; // Ambient occlusion output
	static unsigned int compositeOutput; // Ambient occlusion applied on hdr input

	static void ambientOcclusionPass(unsigned int depthInput);
	static void compositePass(unsigned int hdrInput);

	static Shader* aoPassShader; // Ambient occlusion pass shader
	static Shader* compositeShader; // Composite shader
};