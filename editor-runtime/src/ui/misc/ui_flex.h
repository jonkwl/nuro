#pragma once

#include <cstdint>
#include <imgui.h>

//
// DEPRECATED, BAD CODE!
//

#define FLEX_FULL_WIDTH ImGui::GetContentRegionAvail().x
#define FLEX_FULL_HEIGHT ImGui::GetContentRegionAvail().y

struct ImVec2;

enum class FlexType
{
	ROW,
	COLUMN
};

enum class Justification
{
	START,
	CENTER,
	END,
	EVEN
};

enum class Alignment
{
	START,
	CENTER,
	END
};

struct Margin
{
	float top = 0.0f;
	float right = 0.0f;
	float bottom = 0.0f;
	float left = 0.0f;

	Margin(float top = 0.0f, float right = 0.0f, float bottom = 0.0f, float left = 0.0f) : top(top), right(right), bottom(bottom), left(left) {}
};

struct FlexBuffer
{
	FlexType type;
	float width;
	float height;
	Justification justification;
	Alignment alignment;
	float spacing;
	Margin margin;
};

class UIFlex
{
public:
	static bool debugMode;

	static void beginFlex(ImGuiID id, FlexType type, float width, float height, Justification justification = Justification::CENTER, Alignment alignment = Alignment::CENTER, float spacing = 0.0f, Margin margin = Margin());
	static void endFlex();

private:
	static float mapAlignment(Alignment alignment);
	static ImVec2 getFlexRowSize(float width, float height);

	static FlexBuffer lastFlex;
};