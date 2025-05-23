#pragma once

#include "inspectable.h"

#include "../assetsys/editor_asset.h"

class AssetInspectable : public Inspectable
{
public:
	AssetInspectable(AssetSID assetId);

	void renderStaticContent(ImDrawList& drawList) override;
	void renderDynamicContent(ImDrawList& drawList) override;
private:
	AssetSID assetId;
};