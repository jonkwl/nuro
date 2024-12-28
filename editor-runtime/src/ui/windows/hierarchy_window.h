#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "window_includes.h"

#include "../core/ecs/ecs_collection.h"

struct HierarchyItem {

	HierarchyItem(uint32_t id, EntityContainer entity, std::string name, std::vector<HierarchyItem> children) : id(id), entity(entity), name(name), children(children), expanded(false) {};

	uint32_t id;
	EntityContainer entity;
	std::string name;
	std::vector<HierarchyItem> children;
	bool expanded;

};

class HierarchyWindow : public EditorWindow
{
public:
	HierarchyWindow();

	void render();

private:
	void renderSearch(ImDrawList& drawList);
	void renderEntityItems(ImDrawList& drawList);
	void renderItem(ImDrawList& drawList, HierarchyItem& item, uint32_t indentation);
	void renderDraggedItem(ImDrawList& drawList);

	void buildSceneHierarchy();

	char searchBuffer[128];

	std::vector<HierarchyItem> currentHierarchy;
	uint32_t selectedItemId;
	HierarchyItem* draggedItem;
	glm::vec4 lastDraggedItemPosition;
};