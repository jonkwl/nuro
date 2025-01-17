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
			renderNone();
			break;
		case SelectionType::EMPTY
			renderEmpty();
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

void InsightPanelWindow::renderNone()
{
	UIComponents::label("Nothing selected", EditorUI::getFonts().uiBig);
	UIComponents::label("Select an entity or asset to inspect and edit it.", EditorUI::getFonts().uiHeadline, IM_COL32(210, 210, 255, 255));
}

void InsightPanelWindow::renderEmpty()
{
	UIComponents::label(selected.name, EditorUI::getFonts().uiBig);
	UIComponents::label("Nothing to inspect or edit.", EditorUI::getFonts().uiHeadline, IM_COL32(210, 210, 255, 255));
}
