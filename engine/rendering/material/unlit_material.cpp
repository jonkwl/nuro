#include "unlit_material.h"

UnlitMaterial::UnlitMaterial()
{
	shader = ShaderBuilder::get("unlit");
	texture = nullptr;
}

void UnlitMaterial::bind()
{
	shader->use();
	texture->use();
}
