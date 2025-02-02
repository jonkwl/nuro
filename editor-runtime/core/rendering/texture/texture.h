#pragma once

#include <cstdint>
#include <string>

enum class TextureType
{
	ALBEDO,
	ROUGHNESS,
	METALLIC,
	NORMAL,
	OCCLUSION,
	EMISSIVE,
	HEIGHT,
	IMAGE_RGB,
	IMAGE_RGBA
};

class Texture
{
public:
	Texture(); // Default constructor returning empty texture

	void bind(uint32_t unit) const; // Bind texture for backend
	void destroy(); // Delete texture in backend

	uint32_t getBackendId() const; // Returns the textures backend id

public:
	static Texture empty(); // Get empty texture
	static Texture load(const std::string& path, TextureType type); // Create texture of type from image data
	static Texture fromBackendId(uint32_t backendId); // Create texture with existing backend texture attached

private:
	explicit Texture(uint32_t backendId); // Construct texture class by backend texture id (0 for none)

	uint32_t backendId; // Backend texture id
};