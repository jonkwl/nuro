#pragma once

struct ImVec2;

enum ItemAlignment {
	ITEMS_LEFT,
	ITEMS_CENTERED,
	ITEMS_RIGHT,
	ITEMS_SPREAD
};

enum SizeShortcuts {
	FULL_WIDTH = -1,
	FULL_HEIGHT = -1
};

struct LayoutElementBuffer {
	ItemAlignment itemAlignment;
	bool showBoundaries;
};

class UILayout
{
public:
	static void beginRow(const char* name, float width, float height, ItemAlignment itemAlignment, float verticalAlignment, float spacing, bool showBoundaries = false);
	static void endRow();
private:
	static ImVec2 getRowSize(float width, float height);

	static LayoutElementBuffer lastLayoutElement;
	static void setLastLayoutElement(ItemAlignment itemAlignment, bool showBoundaries);
};

