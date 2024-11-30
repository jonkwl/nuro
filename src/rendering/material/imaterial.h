#pragma once

#include "../src/rendering/shader/shader.h"

class IMaterial
{
public:
	virtual ~IMaterial() {}

	virtual void bind() {}
	virtual Shader* getShader() { return nullptr; }
};