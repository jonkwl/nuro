#pragma once

#include <deque>

#include "../src/ui/editor_window.h"

class DiagnosticsWindow : public EditorWindow
{
public:
	DiagnosticsWindow();

	void render();

private:
	std::deque<float> fpsCache;
	float fpsUpdateTimer;

	float remap(float old_min, float old_max, float new_min, float new_max, float value);
};
