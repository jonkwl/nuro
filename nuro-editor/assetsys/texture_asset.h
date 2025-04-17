#pragma once

#include "editor_asset.h"

#include <memory/resource_manager.h>
#include <rendering/texture/texture.h>

enum class TextureType;

class TextureAsset : public EditorAsset {
public:
	TextureAsset();
	~TextureAsset() override;

	void onDefaultLoad() override;
	void onUnload() override;

	void renderInspectableUI() override;

	uint32_t icon() const override;

private:
	ResourceRef<Texture> textureResource;

	void load(TextureType type);
};