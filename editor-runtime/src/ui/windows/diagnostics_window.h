#pragma once

#include <deque>

#include "editor_window.h"

class DiagnosticsWindow : public EditorWindow
{
public:
	DiagnosticsWindow();

	void render() override;

private:
	std::deque<float> fpsCache;
	float fpsUpdateTimer;
};
