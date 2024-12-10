#pragma once

#include <string>

enum class TextureType
{
	ALBEDO,
	NORMAL,
	ROUGHNESS,
	METALLIC,
	OCCLUSION,
	EMISSIVE,
	IMAGE_RGB,
	IMAGE_RGBA
};

class Texture
{
public:
	Texture(); // Default constructor returning empty texture

	void bind(unsigned int unit); // Bind texture for backend
	void destroy(); // Delete texture in backend

	unsigned int getBackendId(); // Returns the textures backend id

public:
	static Texture empty(); // Get empty texture
	static Texture load(std::string path, TextureType type); // Create texture of type from image data
	static Texture fromBackendId(unsigned int backendId); // Create texture with existing backend texture attached

private:
	explicit Texture(unsigned int backendId); // Construct texture class by backend texture id (0 for none)

	unsigned int backendId; // Backend texture id
};