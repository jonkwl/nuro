#pragma once

#include <string>

typedef unsigned int GLuint;

enum class TextureType
{
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
	Texture();					 // Construct without existing backend texture attached
	explicit Texture(GLuint id); // Construct with existing backend texture attached

	void bind(unsigned int unit); // Bind texture for backend
	void destroy();				  // Delete texture in backend

public:
	static Texture load(std::string path, TextureType type); // Create texture of type from image data

private:
	GLuint id; // Backend texture id
};