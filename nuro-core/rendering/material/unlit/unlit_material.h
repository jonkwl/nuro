#pragma once

#include "../imaterial.h"

#include <viewport/viewport.h>
#include <memory/resource_manager.h>

/* !!!!!!!!!!!!!!!!!!!!!!!!!!
   !!					   !!
   !! BAD TEMPORARY CODE!  !!
   !!					   !!
   !!!!!!!!!!!!!!!!!!!!!!!!!! */

class UnlitMaterial : public IMaterial
{
public:
	UnlitMaterial();

	void bind() const override;
	uint32_t getId() const override;
	ResourceRef<Shader> getShader() const override;
	uint32_t getShaderId() const override;

	glm::vec4 baseColor;
	glm::vec2 tiling;
	glm::vec2 offset;

private:
	uint32_t id;
	ResourceRef<Shader> shader;
	uint32_t shaderId;
};
