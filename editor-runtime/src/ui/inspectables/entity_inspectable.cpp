#include "entity_inspectable.h"

#include "../src/ui/editor_ui.h"
#include "../src/ui/ui_components.h"
#include "../src/ui/windows/registry_window.h"
#include "../src/ui/windows/insight_panel_window.h"

EntityInspectable::EntityInspectable(HierarchyItem& item) : item(item)
{
}

std::string EntityInspectable::getName()
{
	return item.entity.name;
}

void EntityInspectable::renderStaticContent(ImDrawList& drawList)
{
	UIComponents::label(item.entity.name, EditorUI::getFonts().h3_bold);
	UIComponents::buttonBig("Add Component");
}

void EntityInspectable::renderDynamicContent(ImDrawList& drawList)
{
	InsightPanelWindow::renderComponent(drawList);
	InsightPanelWindow::renderComponent(drawList);
	InsightPanelWindow::renderComponent(drawList);
	InsightPanelWindow::renderComponent(drawList);
	InsightPanelWindow::renderComponent(drawList);
	InsightPanelWindow::renderComponent(drawList);
	InsightPanelWindow::renderComponent(drawList);
}