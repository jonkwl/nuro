#pragma once

#include <deque>

#include "window_includes.h"

class DiagnosticsWindow : public EditorWindow
{
public:
	DiagnosticsWindow();

	void render() override;

private:
	std::deque<float> fpsCache;
	float fpsUpdateTimer;
};
