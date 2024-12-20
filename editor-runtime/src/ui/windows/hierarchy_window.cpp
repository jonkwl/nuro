#include "hierarchy_window.h"
#include "window_includes.h"

#include "../core/ecs/ecs_collection.h"

HierarchyWindow::HierarchyWindow() : searchBuffer("")
{
}

void HierarchyWindow::render()
{
	ImGui::Begin(UIUtils::windowTitle("Hierarchy"), nullptr, EditorUI::getWindowFlags().fixed);

	UIComponents::headline("Hierarchy", ICON_FA_SITEMAP, HeadlineJustification::LEFT);

	renderSearch();
	renderItems();

	ImGui::End();
}

void HierarchyWindow::renderSearch()
{
	UIComponents::space(0.0f, 10.0f);
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
	if (ImGui::InputTextWithHint("##Search", "Search...", searchBuffer, IM_ARRAYSIZE(searchBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
		Log::printWarning("Hierarchy Window", "Search not implemented yet");
	}
	ImGui::PopItemWidth();
	UIComponents::space(0.0f, 10.0f);
}

void HierarchyWindow::renderItems()
{

	auto transforms = ECS::registry.view<TransformComponent>();
	unsigned int i = 0;
	for (auto [entity, transform] : transforms.each()) {

		i++;

		if (ImGui::TreeNodeEx(("Entity " + std::to_string(i)).c_str())) {

			ImGui::TreePop();

		}

	}

}