#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

#include "editor_window.h"

#include "../core/ecs/ecs_collection.h"

struct HierarchyItem {

	HierarchyItem(uint32_t id, EntityContainer entity, std::vector<HierarchyItem> children) : id(id), entity(entity), children(children), expanded(false) {};

	uint32_t id;
	EntityContainer entity;
	std::vector<HierarchyItem> children;
	bool expanded;

	bool operator==(const HierarchyItem& other) {
		return id == other.id;
	}

};

class RegistryWindow : public EditorWindow
{
public:
	RegistryWindow();

	void render() override;

private:
	// Renders the search bar
	void renderSearch(ImDrawList& drawList);

	// Renders the current hierarchy as a whole
	void renderHierarchy(ImDrawList& drawList);

	// Renders a given item and all of its children
	void renderItem(ImDrawList& drawList, HierarchyItem& item, uint32_t indentation);

	// Renders the indicator for the item currently dragged
	void renderDraggedItem();

	// Renders the context menu
	void renderContextMenu();

	// Updates camera movement
	void updateCameraMovement();

	// Builds the opened scene into a hierarchy
	void buildSceneHierarchy();

	// Sets a new target for the camera movement
	void setCameraTarget(TransformComponent* target);

	// Performs vertical auto scroll if an item is dragged
	void performAutoScroll();

	char searchBuffer[128];

	// Set if the context menu was used within the last frame
	bool contextMenuUsed;

	// Current hierarchy
	std::vector<HierarchyItem> currentHierarchy;

	// Map of pointers to the currently selected items of the hierarchy sorted by their id
	std::unordered_map<uint32_t, HierarchyItem*> selectedItems;

	HierarchyItem* lastSelected;
	HierarchyItem* lastHovered;

	// Rect for dragged hierarchy items
	DynamicContent dragRect;
	bool draggingHierarchy;

	bool cameraMoving;
	float cameraMovementTime;
	TransformComponent* cameraTarget;
};