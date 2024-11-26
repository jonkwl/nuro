#pragma once

#include <string>

enum class TextureType {
	ALBEDO,
	NORMAL,
	ROUGHNESS,
	METALLIC,
	AMBIENT_OCCLUSION,
	EMISSIVE
};

class Texture
{
public:
	Texture(std::string path, TextureType type);

	TextureType type;

	void bind(unsigned int unit);
private:
	unsigned int id;
};