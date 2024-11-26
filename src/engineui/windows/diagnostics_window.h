#pragma once

#include <deque>

#include "../src/engineui/engine_window.h"

class DiagnosticsWindow : public EngineWindow
{
public:
	void prepare();
private:
	static std::deque<float> fpsCache;
	static float fpsUpdateTimer;
};

