#include "unlit_material.h"

#include <utils/console.h>
#include <rendering/texture/texture.h>
#include <rendering/shader/shader_pool.h>

/* !!!!!!!!!!!!!!!!!!!!!!!!!!
   !!					   !!
   !! BAD TEMPORARY CODE!  !!
   !!					   !!
   !!!!!!!!!!!!!!!!!!!!!!!!!! */

UnlitMaterial::UnlitMaterial() : baseColor(1.0f, 1.0f, 1.0f, 1.0f),
tiling(1.0f, 1.0f),
offset(0.0f, 0.0f),
id(0),
shader(ShaderPool::get("unlit")),
shaderId(0)
{
	shaderId = shader->backendId();
}

void UnlitMaterial::bind() const
{
	if (!shader) return;

	shader->bind();

	shader->setVec4("baseColor", baseColor);

	shader->setVec2("tiling", tiling);
	shader->setVec2("offset", offset);
}

uint32_t UnlitMaterial::getId() const
{
	return id;
}

ResourceRef<Shader> UnlitMaterial::getShader() const
{
	return shader;
}

uint32_t UnlitMaterial::getShaderId() const
{
	return shaderId;
}
