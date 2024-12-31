#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "window_includes.h"

#include "../core/ecs/ecs_collection.h"

struct HierarchyItem {

	HierarchyItem(uint32_t id, EntityContainer entity, std::vector<HierarchyItem> children) : id(id), entity(entity), children(children), expanded(false) {};

	uint32_t id;
	EntityContainer entity;
	std::vector<HierarchyItem> children;
	bool expanded;

};

class HierarchyWindow : public EditorWindow
{
public:
	HierarchyWindow();

	void render();

private:
	// Renders the search bar
	void renderSearch(ImDrawList& drawList);

	// Renders the current hierarchy as a whole
	void renderHierarchy(ImDrawList& drawList);

	// Renders a given item and all of its children
	void renderItem(ImDrawList& drawList, HierarchyItem& item, uint32_t indentation);

	// Renders the indicator for the item currently dragged
	void renderDraggedItem();

	// Updates camera movement
	void updateCameraMovement();

	// Builds the opened scene into a hierarchy
	void buildSceneHierarchy();

	// Sets a new target for the camera movement
	void setCameraTarget(TransformComponent* target);

	// Performs vertical auto scroll if an item is dragged
	void performAutoScroll();

	char searchBuffer[128];

	std::vector<HierarchyItem> currentHierarchy;
	uint32_t selectedItemId;

	UIContentRect dragRect;
	HierarchyItem* draggedItem;

	bool cameraMoving;
	float cameraMovementTime;
	TransformComponent* cameraTarget;
};