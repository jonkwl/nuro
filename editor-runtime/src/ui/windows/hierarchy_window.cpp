#include "hierarchy_window.h"

#include "../core/ecs/ecs_collection.h"

HierarchyWindow::HierarchyWindow() : searchBuffer("")
{
}

void HierarchyWindow::render()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
	ImGui::Begin(UIUtils::windowTitle("Hierarchy"), nullptr, EditorFlags::fixed);

	// Get draw list
	ImDrawList& drawList = *ImGui::GetWindowDrawList();

	UIComponents::headline("Hierarchy", ICON_FA_SITEMAP, HeadlineJustification::LEFT);

	renderSearch(drawList);
	renderEntityItems(drawList);

	ImGui::End();
	ImGui::PopStyleVar();
}

void HierarchyWindow::renderSearch(ImDrawList& drawList)
{
	const float padding = 8.0f;
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(padding, padding));
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

	if (ImGui::InputTextWithHint("##Search", "Search...", searchBuffer, IM_ARRAYSIZE(searchBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
		Log::printWarning("Hierarchy Window", "Search not implemented yet");
	}

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();
	UIComponents::space(0.0f, 5.0f);
}

void HierarchyWindow::renderEntityItems(ImDrawList& drawList)
{

	auto transforms = ECS::gRegistry.view<TransformComponent>();
	uint32_t i = 0;
	for (auto [entity, transform] : transforms.each()) {
		i++;
	}

	renderItem(drawList, "Cube (1)", 0, false, false, false);
	renderItem(drawList, "Cube (2)", 0, true, true, true);
	renderItem(drawList, "Cube (3)", 1, false, false, false);
	renderItem(drawList, "Cube (4)", 1, true, true, false);
	renderItem(drawList, "Cube (5)", 2, false, false, false);
	renderItem(drawList, "Ground", 0, false, false, false);
	renderItem(drawList, "Player", 0, true, true, false);
	renderItem(drawList, "Light", 1, false, false, false);

}

void HierarchyWindow::renderItem(ImDrawList& drawList, std::string name, uint32_t indentation, bool children, bool opened, bool selected)
{
	// Setup
	const float indentationOffset = 30.0f;
	const ImVec2 textPadding = ImVec2(10.0f, 6.5f);

	const float textHeight = ImGui::GetFontSize();
	const float textOffset = indentation * indentationOffset;
	const char* caret = children ? (opened ? " " ICON_FA_CARET_DOWN : " " ICON_FA_CARET_RIGHT) : "";

	// Get the current window position and cursor position
	ImVec2 cursorPos = ImGui::GetCursorScreenPos();
	ImVec2 contentRegion = ImGui::GetContentRegionAvail();

	// Draw the filled rectangle
	ImVec2 rectMin = ImVec2(cursorPos.x, cursorPos.y);
	ImVec2 rectMax = ImVec2(cursorPos.x + contentRegion.x, cursorPos.y + textHeight + textPadding.y * 2);
	bool hovered = ImGui::IsMouseHoveringRect(rectMin, rectMax);
	ImU32 color = selected ? EditorColors::selection : (hovered ? UIUtils::lighten(EditorColors::background, 0.6f) : EditorColors::background);
	drawList.AddRectFilled(rectMin, rectMax, color, 5.0f);

	// Check for selection
	if (ImGui::IsMouseClicked(0) && ImGui::GetMousePos().x >= rectMin.x && ImGui::GetMousePos().x <= rectMax.x && ImGui::GetMousePos().y >= rectMin.y && ImGui::GetMousePos().y <= rectMax.y) {
		Log::printProcessInfo("Clicked " + name);
	}
	
	// Draw text
	ImVec2 textPos = ImVec2(rectMin.x + textPadding.x + textOffset, rectMin.y + textPadding.y);
	std::string textValue = std::string(caret + ("  " + name));
	drawList.AddText(textPos, EditorColors::text, textValue.c_str());

	// Advance cursor
	ImGui::Dummy(ImVec2(contentRegion.x, rectMax.y - rectMin.y - EditorSizing::framePadding * 2 + 2.0f));
}