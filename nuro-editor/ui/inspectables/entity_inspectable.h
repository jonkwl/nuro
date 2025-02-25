#pragma once

#include <functional>

#include "inspectable.h"

#include <ecs/ecs_collection.h>
#include "../ui/windows/registry_window.h"

class EntityInspectable : public Inspectable
{
public:
	EntityInspectable(HierarchyItem& item);

	void renderStaticContent(ImDrawList& drawList) override;
	void renderDynamicContent(ImDrawList& drawList) override;
private:
	// Currently inspected hierarchy item
	HierarchyItem& item;
};