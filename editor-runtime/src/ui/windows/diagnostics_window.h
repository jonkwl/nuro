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
};
