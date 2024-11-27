#pragma once

#include <deque>

#include "../src/editor/editor_window.h"

class DiagnosticsWindow : public EditorWindow
{
public:
	void prepare();

private:
	static std::deque<float> fpsCache;
	static float fpsUpdateTimer;
};
