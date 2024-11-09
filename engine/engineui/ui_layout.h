#pragma once

#include "../engine/utils/log.h"

struct ImVec2;

enum FlexType {
	ROW,
	COLUMN
};

enum Justification {
	JUSTIFY_START,
	JUSTIFY_CENTER,
	JUSTIFY_END,
	JUSTIFY_EVEN
};

enum Alignment {
	ALIGN_START,
	ALIGN_CENTER,
	ALIGN_END
};

enum SizeShortcut {
	FULL_WIDTH = -1,
	FULL_HEIGHT = -1
};

struct Margin {
	float top = 0.0f;
	float right = 0.0f;
	float bottom = 0.0f;
	float left = 0.0f;
};

struct FlexBuffer {
	FlexType type;
	float width;
	float height;
	Justification justification;
	Alignment alignment;
	float spacing;
	Margin margin;
	bool showBoundaries;
};

class UILayout
{
public:
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