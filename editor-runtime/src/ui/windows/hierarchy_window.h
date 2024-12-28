#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "window_includes.h"

struct HierarchyItem {

	HierarchyItem() : id(), handle(), name("NULL"), children(), expanded(false) {};
	HierarchyItem(uint32_t id, entt::entity handle, std::string name, std::vector<HierarchyItem> children) : id(id), handle(handle), name(name), children(children), expanded(false) {};

	uint32_t id;
	entt::entity handle;
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