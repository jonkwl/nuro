#pragma once

#include "../imaterial.h"

#include "../src/core/viewport/viewport.h"
#include "../src/core/rendering/texture/texture.h"

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
	Shader* getShader() const override;
	uint32_t getShaderId() const override;

	glm::vec4 baseColor;
	glm::vec2 tiling;
	glm::vec2 offset;

private:
	uint32_t id;
	Shader* shader;
	uint32_t shaderId;
};
