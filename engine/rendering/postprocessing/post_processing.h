#pragma once

#include <glad/glad.h>
#include <glm.hpp>

#include "../engine/window/window.h"
#include "../engine/utils/log.h"
#include "../engine/rendering/shader/shader.h"
#include "../engine/rendering/shader/shader_builder.h"

class PostProcessing
{
public:
	static void Initialize();
	static void Bind();
	static void Render();

	static float exposure;
	static float contrast;
	static float gamma;

	static bool chromaticAberration;
	static float chromaticAberrationStrength;
	static float chromaticAberrationRange;
	static float chromaticAberrationRedOffset;
	static float chromaticAberrationBlueOffset;

	static bool vignette;
	static glm::vec4 vignetteColor;
	static float vignetteRadius;
	static float vignetteSoftness;
	static float vignetteRoundness;
private:
	static unsigned int fbo;
	static unsigned int rbo;
	static unsigned int texture;
	static unsigned int vao;
	static unsigned int vbo;
	static Shader* shader;
};

