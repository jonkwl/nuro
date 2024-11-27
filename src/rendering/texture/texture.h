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
	Texture() : id(0) {}								  // Construct with no backend texture attached
	explicit Texture(GLuint id) : id(id) {}				  // Construct with existing backend texture attached
	explicit Texture(std::string path, TextureType type); // Construct with newly from image created backend texture attached

	void bind(unsigned int unit); // Bind texture for backend

	void destroy(); // Delete texture in backend
private:
	GLuint id; // Backend texture id
};