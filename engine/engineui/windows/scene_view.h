#pragma once

#include "../engine/engineui/engine_window.h"
#include "../engine/engineui/engine_ui.h"

class SceneView : public EngineWindow
{
public:
	void prepare();

private:
	bool wireframe;
	bool solidMode;
	bool shadows;
};

