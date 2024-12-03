#pragma once

#include "../core/viewport/viewport.h"
#include "../core/rendering/shader/shader.h"

class IMaterial
{
public:
	virtual ~IMaterial() {}

	virtual void bind() {}
	virtual Shader* getShader() { return nullptr; }
};