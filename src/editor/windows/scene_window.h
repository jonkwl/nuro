#pragma once

#include <glm.hpp>

#include "../src/editor/editor_window.h"

class SceneWindow : public EditorWindow
{
public:
	void prepare();
private:
	glm::vec2 lastContentRegionAvail; // Cache for last frames content region avail
};