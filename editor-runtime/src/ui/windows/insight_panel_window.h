#pragma once

#include <string>

#include "window_includes.h"

enum class SelectionType {
	NONE,
	ENTITY,
	MARKDOWN_ASSET,
	MATERIAL_ASSET,
	MODEL_ASSET,
	LEVEL_ASSET
};

struct SelectionBuffer {
	SelectionType type = SelectionType::NONE;
	std::string name = "None";
};

class InsightPanelWindow : public EditorWindow
{
public:
	InsightPanelWindow();

	void render();

	static void setSelected(SelectionBuffer selected);

private:
	static SelectionBuffer selected;

	// Renders the insight panels headline
	void renderHeadline();

	// Renders the current selected as a none selection type
	void renderSelectionNone();

	// Renders the given component component
	void renderComponent(ImDrawList& drawList);

	// Renders the given image image
	void renderImage(uint32_t textureId, float aspectRatio, std::array<float, 2> margin = { 0.0f, 15.0f }, bool stickToBottom = false);

};