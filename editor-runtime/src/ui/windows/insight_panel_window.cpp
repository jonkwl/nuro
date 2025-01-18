#include "insight_panel_window.h"

#include "../core/input/cursor.h"

std::string InsightPanelWindow::headline;
Inspectable* InsightPanelWindow::inspected;

InsightPanelWindow::InsightPanelWindow() : previewViewerHeight(300.0f)
{
}

void InsightPanelWindow::render()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
	ImGui::Begin(UIUtils::windowTitle("Insight Panel"), nullptr, EditorFlag::fixed);
	{
		// Get draw list
		ImDrawList& drawList = *ImGui::GetWindowDrawList();

		// Render headline
		renderHeadline();

		// Rendering preview
		bool renderingPreview = true;

		// Get sizes of content (rendered by inspected) and preview viewer
		ImVec2 contentSize = ImGui::GetContentRegionAvail();
		ImVec2 previewSize = ImVec2(0.0f, 0.0f);

		// Adjust sizing if preview viewer is being rendered
		if (renderingPreview) {
			// Get preview viewer size
			previewSize = getPreviewViewerSize();

			// Subtract preview viewer size from content size
			contentSize.y -= previewSize.y;

			// Remove window padding from preview size
			previewSize -= -ImVec2(0.0f, EditorSizing::windowPadding);
		}

		// Render inspected if available
		if (inspected) {

			// Add margin before rendering inspected
			ImVec2 margin = ImVec2(0.0f, 10.0f);
			ImGui::Dummy(margin);

			// Inspected content child
			UIComponents::beginChild(contentSize - margin);
			{
				inspected->render(drawList);
			}
			UIComponents::endChild();

			// Render preview viewer if available
			if (renderingPreview) renderPreviewViewer(drawList, previewSize);

		}
		// Otherwise render none inspected
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

void InsightPanelWindow::renderPreviewViewer(ImDrawList& drawList, ImVec2 size)
{
	// tmp
	uint32_t source = 0;

	// 
	// RE-RENDER PREVIEW IF NEEDED
	//

	//

	//
	// CREATE TOP BAR
	//

	// Evaluate top bar sizing
	const ImVec2 xWindowPadding = ImVec2(20.0f, 0.0f);
	ImVec2 topBarPos = ImGui::GetCursorScreenPos() - xWindowPadding;
	ImVec2 topBarSize = ImVec2(ImGui::GetContentRegionAvail().x, 1.0f) + xWindowPadding * 2;
	
	// Evalute top bar interactions
	float hoverArea = 8.0f;
	bool topBarHovered = ImGui::IsMouseHoveringRect(topBarPos - ImVec2(0.0f, hoverArea), topBarPos + ImVec2(topBarSize.x, hoverArea));
	bool topBarInteracted = topBarHovered && ImGui::IsMouseClicked(0);

	// Set vertical resize cursor if needed
	if (topBarHovered) {
		EditorUI::setCursorType(CursorType::RESIZE_VERTICAL);
	}
	
	// Start top bar drag if interacted with top bar and not actively dragged yet
	static bool topBarActiveDrag = false;
	if (topBarInteracted && !topBarActiveDrag) {
		topBarActiveDrag = true;
	}

	// If top bar is currently being dragged
	if (topBarActiveDrag) {
		// Add vertical cursor offset to preview viewer height
		previewViewerHeight = (ImGui::GetWindowPos().y + ImGui::GetWindowSize().y) - ImGui::GetMousePos().y;

		// If mouse button isnt hold down anymore, stop drag
		if (!ImGui::IsMouseDown(0)) {
			topBarActiveDrag = false;
		}
	}

	// Draw top bar
	drawList.AddRectFilled(topBarPos, topBarPos + topBarSize, topBarHovered ? EditorColor::selection : EditorColor::selectionInactive, 6.0f);

	//
	// DRAW RENDERED PREVIEW
	//

	ImVec2 imagePos = topBarPos + ImVec2(0.0f, topBarSize.y);
	ImVec2 imageSize = size - ImVec2(0.0f, topBarSize.y) + xWindowPadding * 2;
	drawList.AddImage(source, imagePos, imagePos + imageSize, ImVec2(0, 1), ImVec2(1, 0));
}

ImVec2 InsightPanelWindow::getPreviewViewerSize()
{
	float xSize = ImGui::GetContentRegionAvail().x;
	float ySize = std::clamp(previewViewerHeight, 50.0f, std::max(50.0f, ImGui::GetWindowSize().y * 0.5f));
	ImVec2 size = ImVec2(xSize, ySize);
	return size;
}  