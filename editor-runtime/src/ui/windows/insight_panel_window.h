#pragma once

#include <string>

#include "window_includes.h"

class InsightPanelWindow : public EditorWindow
{
public:
	InsightPanelWindow();

	void render() override;

	// Deletes previously inspected inspectable and sets given inspectable as inspected
	static void inspect(const std::string& headline, Inspectable* inspectable);

public:
	// Renders the given component component
	static void renderComponent(ImDrawList& drawList);

	// Renders an image by the given backend texture id
	static void renderImage(uint32_t textureId, float aspectRatio, std::array<float, 2> margin = { 0.0f, 15.0f });

private:
	// Current headline
	static std::string headline;

	// Currently inspected object
	static Inspectable* inspected;

private:
	// Renders the insight panels headline
	void renderHeadline();

	// Renders the nothing inspected text
	void renderNoneInspected();

	// Renders preview viewer by given preview renderer
	void renderPreviewViewer(ImVec2 size);

	// Returns the size of the preview viewer size
	ImVec2 getPreviewViewerSize();

};