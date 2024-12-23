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
	renderItems(drawList);

	ImGui::End();
}

void HierarchyWindow::renderSearch(ImDrawList& drawList)
{
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
	if (ImGui::InputTextWithHint("##Search", "Search...", searchBuffer, IM_ARRAYSIZE(searchBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
		Log::printWarning("Hierarchy Window", "Search not implemented yet");
	}
	ImGui::PopItemWidth();
	UIComponents::space(0.0f, 5.0f);
}

void HierarchyWindow::renderItems(ImDrawList& drawList)
{

	auto transforms = ECS::gRegistry.view<TransformComponent>();
	uint32_t i = 0;
	for (auto [entity, transform] : transforms.each()) {

		i++;

		if (ImGui::TreeNodeEx(("Entity " + std::to_string(i)).c_str())) {

			ImGui::TreePop();

		}

	}

}

void HierarchyWindow::renderItem(ImDrawList& drawList)
{

}
