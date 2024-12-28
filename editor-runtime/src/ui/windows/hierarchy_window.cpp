#include "hierarchy_window.h"

#include "../core/ecs/ecs_collection.h"

HierarchyWindow::HierarchyWindow() : searchBuffer("")
{
}

void HierarchyWindow::render()
{
	ImGui::Begin(UIUtils::windowTitle("Hierarchy"), nullptr, EditorUI::getWindowFlags().fixed);

	// Get draw list
	ImDrawList& drawList = *ImGui::GetWindowDrawList();

	UIComponents::headline("Hierarchy", ICON_FA_SITEMAP, HeadlineJustification::LEFT);

	renderSearch(drawList);
	renderEntityItems(drawList);

	ImGui::End();
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
		renderItem(drawList, "Entity " + std::to_string(i));
	}

}

void HierarchyWindow::renderItem(ImDrawList& drawList, std::string name)
{
	// Setup
	const float itemHeight = 20.0f;
	const float textPadding = 5.0f;
	const float bottomMargin = 0.0f;

	// Get the current window position and cursor position
	ImVec2 cursorPos = ImGui::GetCursorScreenPos();
	ImVec2 contentRegion = ImGui::GetContentRegionAvail();

	// Draw the filled rectangle
	ImVec2 rectMin = cursorPos;
	ImVec2 rectMax = ImVec2(cursorPos.x + contentRegion.x, cursorPos.y + itemHeight);
	bool hovered = ImGui::IsMouseHoveringRect(rectMin, rectMax);
	ImU32 color = hovered ? EditorUI::getColors().element : EditorUI::getColors().elementHovered;
	drawList.AddRectFilled(rectMin, rectMax, color);

	// Draw text
	float textHeight = ImGui::GetFontSize();
	ImVec2 textPos = ImVec2(rectMin.x + textPadding, rectMin.y + (itemHeight - textHeight) * 0.5f);
	drawList.AddText(textPos, EditorUI::getColors().text, name.c_str());

	// Advance cursor
	ImGui::Dummy(ImVec2(contentRegion.x, itemHeight + bottomMargin));
}