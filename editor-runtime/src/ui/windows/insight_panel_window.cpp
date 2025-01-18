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
	ImDrawList& drawList = *ImGui::GetWindowDrawList();

	UIComponents::label("Nothing selected", EditorUI::getFonts().h3_bold);
	UIComponents::label("Select an entity or asset to inspect and edit it.", EditorUI::getFonts().h4, IM_COL32(210, 210, 255, 255));

	renderComponent(drawList);
}

void InsightPanelWindow::renderComponent(ImDrawList& drawList)
{
	static bool collapsed = true;

	// Fetch data
	ImVec2 contentAvail = ImGui::GetContentRegionAvail();
	ImVec2 cursorPos = ImGui::GetCursorScreenPos();

	// Evaluate size
	float titleHeight = EditorSizing::h4_FontSize;
	ImVec2 titlePadding = ImVec2(10.0f, 10.0f);
	float collapsedHeight = titleHeight + titlePadding.y * 2;
	float expandedHeight = 250.0f;
	ImVec2 size = ImVec2(contentAvail.x, collapsed ? collapsedHeight : expandedHeight);

	// Draw background
	float yMargin = 10.0f;
	ImVec2 p0 = ImVec2(cursorPos.x, cursorPos.y + yMargin);
	ImVec2 p1 = ImVec2(p0.x + size.x, p0.y + size.y);
	drawList.AddRectFilled(p0, p1, EditorColor::element, 10.0f);

	ImVec2 titlePos = p0 + titlePadding;
	std::string title = std::string(collapsed ? ICON_FA_CARET_RIGHT : ICON_FA_CARET_DOWN) + "   " + selected.name;
	if (UIUtils::caretBackend(drawList, titlePos, ImVec2(-1.0f, 2.0f), EditorColor::element, EditorColor::elementActive)) {
		collapsed = !collapsed;
	}
	drawList.AddText(EditorUI::getFonts().h4_bold, EditorSizing::h4_FontSize, titlePos, EditorColor::text, title.c_str());

	// Advance cursor
	ImGui::Dummy(ImVec2(size.x, size.y + yMargin));
}

void InsightPanelWindow::renderImage(uint32_t textureId, float aspectRatio, std::array<float, 2> margin, bool stickToBottom)
{
	// Add margin
	ImVec2 cursorPos = ImGui::GetCursorPos();
	cursorPos.x += margin[0];
	cursorPos.y += margin[1];
	ImGui::SetCursorPos(cursorPos);

	// Draw image
	float xAvail = ImGui::GetContentRegionAvail().x - margin[0];
	ImVec2 size = ImVec2(xAvail, xAvail / aspectRatio);
	ImGui::Image(textureId, size, ImVec2(0, 1), ImVec2(1, 0));

	// Add bottom margin
	cursorPos = ImGui::GetCursorPos();
	cursorPos.y += margin[1];
	ImGui::SetCursorPos(cursorPos);
}
