#include "insight_panel_window.h"

SelectionBuffer InsightPanelWindow::selected;

InsightPanelWindow::InsightPanelWindow()
{
}

void InsightPanelWindow::render()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
	ImGui::Begin(UIUtils::windowTitle("Insight Panel"), nullptr, EditorFlag::fixed);
	{
		// Get draw list
		ImDrawList& drawList = *ImGui::GetWindowDrawList();

		renderHeadline();

		switch (selected.type) {
		case SelectionType::NONE:
			renderSelectionNone();
			break;
		default:
			break;
		}
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void InsightPanelWindow::setSelected(SelectionBuffer _selected)
{
	selected = _selected;
}

void InsightPanelWindow::renderHeadline()
{
	UIComponents::headline("Insight Panel", ICON_FA_LAYER_GROUP);
}

void InsightPanelWindow::renderSelectionNone()
{
	UIComponents::label("Nothing selected", EditorUI::getFonts().h3_bold);
	UIComponents::label("Select an entity or asset to inspect and edit it.", EditorUI::getFonts().h4, IM_COL32(210, 210, 255, 255));
}

void InsightPanelWindow::renderComponent()
{
}

void InsightPanelWindow::renderImage(uint32_t textureId, float aspectRatio, std::array<float, 2> margin, bool stickToBottom)
{
	// Add margin
	ImVec2 cursorPos = ImGui::GetCursorPos();
	cursorPos.x += margin[0];
	cursorPos.y += margin[1] * 0.5f;
	ImGui::SetCursorPos(cursorPos);

	// Draw image
	float xAvail = ImGui::GetContentRegionAvail().x - margin[0];
	ImVec2 size = ImVec2(xAvail, xAvail / aspectRatio);
	ImGui::Image(textureId, size, ImVec2(0, 1), ImVec2(1, 0));

	// Add bottom margin
	cursorPos = ImGui::GetCursorPos();
	cursorPos.y += margin[1] * 0.5f;
	ImGui::SetCursorPos(cursorPos);
}
