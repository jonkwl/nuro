#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <glm.hpp>

#include "../src/core/time/time.h"
#include "../src/core/utils/console.h"

#include "../src/ui/editor_ui.h"
#include "../src/runtime/runtime.h"
#include "../src/ui/utils/ui_utils.h"
#include "../src/ui/inspectables/inspectable.h"
#include "../src/ui/components/im_components.h"
#include "../src/ui/context_menu/context_menu.h"
#include "../src/ui/collection/IconsFontAwesome6.h"
#include "../src/ui/dynamic_drawing/dynamic_drawing.h"

class EditorWindow
{
public:
	virtual void render() {};
};
