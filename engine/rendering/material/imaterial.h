#pragma once

#include "../engine/rendering/shader/shader.h"

namespace {
	constexpr int SHADOW_MAP_UNIT = 0;
	constexpr int ALBEDO_MAP_UNIT = 1;
	constexpr int NORMAL_MAP_UNIT = 2;
	constexpr int ROUGHNESS_MAP_UNIT = 3;
	constexpr int METALLIC_MAP_UNIT = 4;
	constexpr int AMBIENT_OCCLUSION_MAP_UNIT = 5;
}

class IMaterial {
public:
	virtual void bind() {}
	virtual ~IMaterial() {}
	virtual Shader* getShader() { return nullptr; }
};