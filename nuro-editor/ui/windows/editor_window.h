#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <glm/glm.hpp>

#include <time/time.h>
#include <utils/console.h>

#include "../ui/editor_ui.h"
#include "../runtime/runtime.h"
#include "../ui/utils/ui_utils.h"
#include "../ui/inspectables/inspectable.h"
#include "../ui/components/im_components.h"
#include "../ui/context_menu/context_menu.h"
#include "../ui/collection/IconsFontAwesome6.h"
#include "../ui/dynamic_drawing/dynamic_text.h"
#include "../ui/dynamic_drawing/dynamic_content.h"

class EditorWindow
{
public:
	virtual void render() {};
};
