#include "unlit_material.h"

#include "../src/rendering/texture/texture.h"
#include "../src/rendering/shader/shader_pool.h"
#include "../src/utils/log.h"

UnlitMaterial::UnlitMaterial() : shader(ShaderPool::get("unlit")),
baseColor(1.0f, 1.0f, 1.0f, 1.0f),
tiling(1.0f, 1.0f),
offset(0.0f, 0.0f),
useTexture(false),
texture(Texture::empty())
{
}

void UnlitMaterial::bind()
{
	shader->bind();

	shader->setVec4("baseColor", baseColor);

	shader->setVec2("tiling", tiling);
	shader->setVec2("offset", offset);

	shader->setBool("enableTexture", useTexture);
	if (useTexture)
	{
		texture.bind(0);
		shader->setInt("baseTexture", 0);
	}
}

Shader* UnlitMaterial::getShader()
{
	return shader;
}
