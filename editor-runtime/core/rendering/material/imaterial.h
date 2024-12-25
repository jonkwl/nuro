#pragma once

#include <cstdint>

#include "../core/viewport/viewport.h"
#include "../core/rendering/shader/shader.h"

class IMaterial
{
public:
	virtual ~IMaterial() {}

	virtual void bind() {}

	virtual uint32_t getId() { return 0; }
	virtual Shader* getShader() { return nullptr; }
	virtual uint32_t getShaderId() { return 0; }
};