#pragma once

#include "../src/viewport/viewport.h"
#include "../src/rendering/shader/shader.h"

class IMaterial
{
public:
	virtual ~IMaterial() {}

	virtual void bind(Viewport& viewport) {}
	virtual Shader* getShader() { return nullptr; }
};