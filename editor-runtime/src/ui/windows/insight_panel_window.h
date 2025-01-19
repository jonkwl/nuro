#pragma once

#include <string>

#include "window_includes.h"

class InsightPanelWindow : public EditorWindow
{
public:
	InsightPanelWindow();

	void render() override;

	// Deletes previously inspected inspectable and sets given inspectable as inspected
	static void inspect(Inspectable* inspectable);

public:
	// Renders the given component component
	static void renderComponent(ImDrawList& drawList);

	// Renders an image by the given backend texture id
	static void renderImage(uint32_t textureId, float aspectRatio, std::array<float, 2> margin = { 0.0f, 15.0f });

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