#include "unlit_material.h"

UnlitMaterial::UnlitMaterial()
{
	shader = ShaderBuilder::get("unlit");
	texture = nullptr;
}

void UnlitMaterial::bind()
{
	shader->bind();

	if (texture != nullptr) {
		texture->bind();
	}
}

Shader* UnlitMaterial::getShader()
{
	return shader;
}
