#include "unlit_material.h"

#include "../engine/runtime/runtime.h"

UnlitMaterial::UnlitMaterial()
{
	shader = ShaderBuilder::get("unlit");
	texture = nullptr;
	baseColor = glm::vec4(1.0f);
}

void UnlitMaterial::bind()
{
	shader->bind();

	bool enableTexture = texture != nullptr;
	shader->setBool("enableTexture", enableTexture);
	if (enableTexture) {
		texture->bind(0);
		shader->setInt("baseTexture", 0);
	}

	shader->setVec4("baseColor", baseColor);
}

Shader* UnlitMaterial::getShader()
{
	return shader;
}
