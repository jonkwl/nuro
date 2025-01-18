#pragma once

#include <string>

#include "inspectable.h"

struct HierarchyItem;

class EntityInspectable : public Inspectable
{
public:
	EntityInspectable(HierarchyItem& item);
	
	std::string getName() override;
	void render(ImDrawList& drawList) override;
private:
	HierarchyItem& item;
};