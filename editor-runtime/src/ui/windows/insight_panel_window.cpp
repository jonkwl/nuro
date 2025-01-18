#include "insight_panel_window.h"

std::string InsightPanelWindow::headline;
Inspectable* InsightPanelWindow::inspected;

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

		bool renderingPreview = false;

		ImVec2 contentSize = ImGui::GetContentRegionAvail();
		ImVec2 previewSize = contentSize;
		if (renderingPreview) {
			previewSize = getPreviewViewerSize();
			contentSize.y -= previewSize.y;
		}

		if (inspected) {

			ImGui::Dummy(ImVec2(0.0f, 10.0f));

			UIComponents::beginChild(contentSize);
			{
				inspected->render(drawList);
			}
			UIComponents::endChild();

			if (renderingPreview) {

				ImVec2 cursorPos = ImGui::GetCursorPos();
				ImVec2 windowPos = ImGui::GetWindowPos();
				ImVec2 windowSize = ImGui::GetWindowSize();
				ImVec2 previewPos = ImVec2(cursorPos.x, windowPos.y + windowSize.y - previewSize.y);

				UIComponents::beginChild(previewSize, previewPos);
				{
					renderPreviewViewer(previewSize);
				}
				UIComponents::endChild();

			}

		}
		else {
			renderNoneInspected();
		}
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void InsightPanelWindow::inspect(const std::string& _headline, Inspectable* inspectable)
{
	// Delete old inspected object
	if (inspected) delete inspected;

	// Set new headline
	headline = _headline;

	// Set new inspected object
	inspected = inspectable;
}

void InsightPanelWindow::renderHeadline()
{
	UIComponents::headline("Insight Panel", ICON_FA_LAYER_GROUP);
	UIComponents::label(headline, EditorUI::getFonts().h3_bold);
}

void InsightPanelWindow::renderComponent(ImDrawList& drawList)
{
	static bool collapsed = true;

	//
	// EVALUATE 
	//

	ImVec2 contentAvail = ImGui::GetContentRegionAvail();
	ImVec2 cursorPos = ImGui::GetCursorScreenPos();

	float titleHeight = EditorSizing::h4_FontSize;
	ImVec2 titlePadding = ImVec2(10.0f, 10.0f);
	
	float collapsedHeight = titleHeight + titlePadding.y * 2;
	float expandedHeight = 250.0f;
	
	float yMargin = 0.5f;
	ImVec2 size = ImVec2(contentAvail.x, collapsed ? collapsedHeight : expandedHeight);
	ImVec2 p0 = ImVec2(cursorPos.x, cursorPos.y + yMargin);
	ImVec2 p1 = ImVec2(p0.x + size.x, p0.y + size.y);

	const bool hovered = ImGui::IsMouseHoveringRect(p0, p1);

	//
	// CLICK COLLAPSE
	//

	if (hovered && ImGui::IsMouseClicked(1)) {
		collapsed = !collapsed;
	}

	//
	// DRAW BACKGROUND
	//

	drawList.AddRectFilled(p0, p1, EditorColor::element, 10.0f);

	//
	// DRAW EXPANSION CARET
	//

	ImVec2 caretPos = p0 + titlePadding;
	if (UIComponents::caret(drawList, caretPos, ImVec2(-1.0f, 2.0f), collapsed ? ICON_FA_CARET_RIGHT : ICON_FA_CARET_DOWN, EditorColor::element, EditorColor::elementActive)) {
		collapsed = !collapsed;
	}

	//
	// DRAW COMPONENT ICON
	//

	ImVec2 iconSize = ImVec2(18.0f, 18.0f);
	ImVec2 iconPos = caretPos + ImVec2(24.0f, -1.0f);
	drawList.AddImage(IconPool::get("example").getBackendId(), iconPos, iconPos + iconSize, ImVec2(0, 1), ImVec2(1, 0));

	//
	// DRAW COMPONENT TEXT
	//

	drawList.AddText(EditorUI::getFonts().h4_bold, EditorSizing::h4_FontSize, iconPos + ImVec2(26.0f, 0.0f), EditorColor::text, "Component Name");

	//
	// ADVANCE CURSOR
	//

	ImGui::Dummy(ImVec2(size.x, size.y + yMargin));
}

void InsightPanelWindow::renderImage(uint32_t textureId, float aspectRatio, std::array<float, 2> margin)
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

void InsightPanelWindow::renderNoneInspected()
{
	headline = "Nothing selected";
	UIComponents::label("Select an entity or asset to inspect and edit it.", EditorUI::getFonts().h4, IM_COL32(210, 210, 255, 255));
}

void InsightPanelWindow::renderPreviewViewer(ImVec2 size)
{
	// tmp
	uint32_t source = IconPool::get("example").getBackendId();

	// 
	// RE-RENDER PREVIEW IF NEEDED
	//

	//

	//
	// DRAW RENDERED PREVIEW
	//

	// ImGui::Image(source, size, ImVec2(0, 1), ImVec2(1, 0));
}

ImVec2 InsightPanelWindow::getPreviewViewerSize()
{
	float xAvail = ImGui::GetContentRegionAvail().x;
	ImVec2 size = ImVec2(xAvail, 250.0f);
	return size;
}