#pragma once

#include <deque>

#include "../editor/editor_window.h"

class DiagnosticsWindow : public EditorWindow
{
public:
	void render();

private:
	static std::deque<float> fpsCache;
	static float fpsUpdateTimer;
};
