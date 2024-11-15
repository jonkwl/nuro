#pragma once

#include "../src/rendering/shader/shader.h"

enum TextureUnit {
    SHADOW_MAP_UNIT,
    ALBEDO_MAP_UNIT,
    NORMAL_MAP_UNIT,
    ROUGHNESS_MAP_UNIT,
    METALLIC_MAP_UNIT,
    AMBIENT_OCCLUSION_MAP_UNIT,
    EMISSION_MAP_UNIT
};

class IMaterial {
public:
	virtual void bind() {}
	virtual ~IMaterial() {}
	virtual Shader* getShader() { return nullptr; }
};