#pragma once

#include <cstdint>

#include "../core/viewport/viewport.h"
#include "../core/rendering/shader/shader.h"

class IMaterial
{
public:
	virtual ~IMaterial() {}

	virtual void bind() const {}

	virtual uint32_t getId() const { return 0; }
	virtual Shader* getShader() const { return nullptr; }
	virtual uint32_t getShaderId() const { return 0; }
};