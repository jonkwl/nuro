#pragma once

#include <cstdint>

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

	static constexpr float FULL_WIDTH = -1.0f;
	static constexpr float FULL_HEIGHT = -1.0f;

	static float defaultWidth;
	static float defaultHeight;
	static Justification defaultJustification;
	static Alignment defaultAlignment;
	static float defaultSpacing;
	static Margin defaultMargin;

	static void beginFlex(const char* name, FlexType type, float width = defaultWidth, float height = defaultHeight, Justification justification = defaultJustification, Alignment alignment = defaultAlignment, float spacing = defaultSpacing, Margin margin = defaultMargin);
	static void endFlex();

private:
	static float mapAlignment(Alignment alignment);
	static ImVec2 getFlexRowSize(float width, float height);

	static FlexBuffer lastFlex;
};