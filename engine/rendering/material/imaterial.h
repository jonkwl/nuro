#pragma once

#include "../engine/rendering/shader/shader.h"

class IMaterial {
public:
	virtual void bind() {}
	virtual ~IMaterial() {}
	virtual Shader* getShader() { return nullptr; }
};