#pragma once

#include <string>

#include "inspectable.h"

struct HierarchyItem;

class EntityInspectable : public Inspectable
{
public:
	EntityInspectable(HierarchyItem& item);
	
	std::string getName() override;

	void renderStaticContent(ImDrawList& drawList) override;
	void renderDynamicContent(ImDrawList& drawList) override;
private:
	HierarchyItem& item;
};