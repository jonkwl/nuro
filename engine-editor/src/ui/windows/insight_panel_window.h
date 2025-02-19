#pragma once

#include <string>
#include <type_traits>

#include "editor_window.h"

class InsightPanelWindow : public EditorWindow
{
public:
	InsightPanelWindow();

	void render() override;

	template <typename T, typename... Args>
	static void inspect(Args&&... args)
	{
		// Make sure provided type is derived from inspectable
		static_assert(std::is_base_of_v<Inspectable, T>, "Only objects which are derived from class 'Inspectable' can be inspected!");

		// Delete old inspected object if existing
		if (inspected) delete inspected;

		// Instantiate new inspectable
		inspected = new T(std::forward<Args>(args)...);
	}

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