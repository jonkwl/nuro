#pragma once

#include "../src/rendering/shader/shader.h"

class IMaterial {
public:
	virtual void bind() {}
	virtual ~IMaterial() {}
	virtual Shader* getShader() { return nullptr; }
};