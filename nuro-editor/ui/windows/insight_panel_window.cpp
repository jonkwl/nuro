#include "insight_panel_window.h"

#include <input/cursor.h>

InsightPanelWindow::InsightPanelWindow() : previewViewerOutput(0),
previewViewerHeight(300.0f),
previewCameraTransform()
{
	previewViewerOutput = Runtime::previewPipeline().createOutput();
}

void InsightPanelWindow::render()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
	std::string title = UIUtils::windowTitle("Insight Panel");
	ImGui::Begin(title.c_str(), nullptr, EditorFlag::fixed);
	{
		// Get draw list
		ImDrawList& drawList = *ImGui::GetWindowDrawList();

		// Render headline
		IMComponents::headline("Insight Panel", ICON_FA_LAYER_GROUP);

		// Render inspected static content if available
		if (inspected) {
			inspected->renderStaticContent(drawList);
		}

		// Rendering preview
		bool renderingPreview = true;

		// Get sizes of content (rendered by inspected) and preview viewer
		ImVec2 contentSize = ImGui::GetContentRegionAvail();
		ImVec2 previewSize = ImVec2(0.0f, 0.0f);

		// Adjust sizing if preview viewer is being rendered
		if (renderingPreview) {
			// Set preview viewer size
			previewSize = ImVec2(ImGui::GetContentRegionAvail().x, previewViewerHeight);

			// Subtract preview viewer size from content size
			contentSize.y -= previewSize.y;

			// Remove window padding from preview size
			previewSize -= -ImVec2(0.0f, EditorSizing::windowPadding);
		}

		// Render inspected dynamic content if available
		if (inspected) {

			// Add margin before rendering inspected
			ImVec2 margin = ImVec2(0.0f, 2.0f);
			ImGui::Dummy(margin);

			// Dynamic inspected content child
			IMComponents::beginClippedChild(contentSize - margin);
			{
				inspected->renderDynamicContent(drawList);
			}
			IMComponents::endClippedChild();

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
	IMComponents::label("Nothing selected", EditorUI::getFonts().h3_bold);
	IMComponents::label("Select an entity or asset to inspect and edit it.", EditorUI::getFonts().h4, IM_COL32(210, 210, 255, 255));
}

void InsightPanelWindow::renderPreviewViewer(ImDrawList& drawList, ImVec2 size)
{
	//
	// CREATE TOP BAR
	//

	// Evaluate top bar sizing
	const ImVec2 xWindowPadding = ImVec2(20.0f, 0.0f);
	ImVec2 topBarPos = ImGui::GetCursorScreenPos() - xWindowPadding;
	ImVec2 topBarSize = ImVec2(ImGui::GetContentRegionAvail().x, 2.0f) + xWindowPadding * 2;
	
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
		// Set preview viewer height according to vertical mouse position
		previewViewerHeight = (ImGui::GetWindowPos().y + ImGui::GetWindowSize().y) - ImGui::GetMousePos().y;

		// Clamp preview viewer height to not exceed limits
		previewViewerHeight = std::clamp(previewViewerHeight, 50.0f, std::max(50.0f, ImGui::GetWindowSize().y * 0.5f));

		// If mouse button isn't hold down anymore, stop dragging
		if (!ImGui::IsMouseDown(0)) {
			topBarActiveDrag = false;
		}
	}

	// Evaluate top bar color
	ImU32 topBarColor = IM_COL32(120, 120, 120, 255);
	if (topBarHovered) topBarColor = IM_COL32(255, 255, 255, 255);
	if (topBarActiveDrag) topBarColor = EditorColor::selection;

	// Draw top bar
	drawList.AddRectFilled(topBarPos, topBarPos + topBarSize, topBarColor, 6.0f);

	// 
	// HANDLE PREVIEW OUTPUT
	//

	// If preview should render this frame
	bool render = true;

	// Get output
	PreviewPipeline& pipeline = Runtime::previewPipeline();
	const PreviewOutput& output = pipeline.getOutput(previewViewerOutput);

	// Get current output size
	glm::vec2 outputSize = glm::vec2(ImGui::GetContentRegionAvail().x, previewViewerHeight);

	// Resize output if current output size doesnt match output viewport size
	if (outputSize != output.viewport.getResolution()) {
		pipeline.resizeOutput(previewViewerOutput, outputSize.x, outputSize.y);
		render = true;
	}

	// Render if needed
	if (render) {
		// tmp
		previewCameraTransform.position = glm::vec3(0.0f, 0.0f, -3.5f);

		// Create and add render instruction
		/*
		PreviewRenderInstruction instruction;
		instruction.backgroundColor = glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);
		instruction.model = MODEL HERE
		instruction.modelMaterial = MATERIAL HERE
		instruction.modelTransform = TransformComponent();
		instruction.cameraTransform = previewCameraTransform;
		pipeline.addRenderInstruction(instruction);
		*/
	}

	// Fetch output texture source
	uint32_t source = output.texture;

	// tmp
	previewOutput = output.texture;

	//
	// DRAW RENDERED PREVIEW
	//

	ImVec2 imagePos = topBarPos + ImVec2(0.0f, topBarSize.y);
	ImVec2 imageSize = size - ImVec2(0.0f, topBarSize.y) + xWindowPadding * 2;
	drawList.AddImage(source, imagePos, imagePos + imageSize, ImVec2(0, 1), ImVec2(1, 0));
}