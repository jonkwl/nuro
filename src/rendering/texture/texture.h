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
	Texture(); // Default constructor returning empty texture

	void bind(unsigned int unit); // Bind texture for backend
	void destroy(); // Delete texture in backend

public:
	static Texture empty(); // Get empty texture
	static Texture load(std::string path, TextureType type); // Create texture of type from image data
	static Texture fromBackendId(GLuint id); // Create texture with existing backend texture attached

private:
	explicit Texture(GLuint id); // Construct texture class by backend texture id (0 for none)

	GLuint id; // Backend texture id
};