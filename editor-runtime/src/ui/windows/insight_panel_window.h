#pragma once

#include <string>

#include "window_includes.h"

enum class SelectionType {
	NONE,
	EMPTY,
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

	void renderHeadline();
	void renderNone();
	void renderEmpty();

};