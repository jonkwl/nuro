#pragma once

#include <string>
#include <functional>

#include "inspectable.h"

#include "../core/ecs/ecs_collection.h"

#include "../src/ui/windows/registry_window.h"

class EntityInspectable : public Inspectable
{
public:
	EntityInspectable(HierarchyItem& item);

	std::string getName() override;
	void renderStaticContent(ImDrawList& drawList) override;
	void renderDynamicContent(ImDrawList& drawList) override;
private:
	// Currently inspected hierarchy item
	HierarchyItem& item;

	// Adds a component to be drawn if its owned by the entity of the currently inspected hierarchy item
	template <typename T>
	void add(std::function<void(T&)> drawFunction) {
		if (item.entity.has<T>()) {
			T& component = item.entity.get<T>();
			drawFunction(component);
		}
	}
};