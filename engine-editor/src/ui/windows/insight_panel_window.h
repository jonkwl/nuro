#pragma once

#include <string>

#include "editor_window.h"

class InsightPanelWindow : public EditorWindow
{
public:
	InsightPanelWindow();

	void render() override;

	// Deletes previously inspected inspectable and sets given inspectable as inspected. Takes ownership of the provided inspectable!
	static void inspect(Inspectable* inspectable);

public:
	// Renders an image by the given backend texture id
	static void renderImage(uint32_t textureId, float aspectRatio, std::array<float, 2> margin = { 0.0f, 15.0f });

	// tmp
	static uint32_t previewOutput;

private:
	// Currently inspected object
	static Inspectable* inspected;

	// Output texture of preview viewer
	size_t previewViewerOutput;

	// Current height of preview viewer
	float previewViewerHeight;

	// Transform of preview viewer camera
	TransformComponent previewCameraTransform;

private:
	// Renders the nothing inspected text
	void renderNoneInspected();

	// Renders preview viewer by given preview renderer
	void renderPreviewViewer(ImDrawList& drawList, ImVec2 size);
};