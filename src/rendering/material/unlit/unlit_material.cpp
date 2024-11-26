#include "unlit_material.h"

#include "../src/runtime/runtime.h"
#include "../src/rendering/texture/texture.h"
#include "../src/rendering/shader/shader_pool.h"
#include "../src/utils/log.h"

UnlitMaterial::UnlitMaterial()
{
	shader = ShaderPool::get("unlit");
	baseColor = glm::vec4(1.0f);
	tiling = glm::vec2(1.0f);
	offset = glm::vec2(0.0f);
	texture = nullptr;
}

void UnlitMaterial::bind()
{
	shader->bind();

	shader->setVec4("baseColor", baseColor);

	shader->setVec2("tiling", tiling);
	shader->setVec2("offset", offset);

	bool enableTexture = texture != nullptr;
	shader->setBool("enableTexture", enableTexture);
	if (enableTexture) {
		texture->bind(0);
		shader->setInt("baseTexture", 0);
	}
}

Shader* UnlitMaterial::getShader()
{
	return shader;
}
