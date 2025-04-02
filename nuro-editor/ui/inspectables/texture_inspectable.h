#pragma once

#include <vector>
#include <string_view>
#include <textselect.hpp>

#include "inspectable.h"

#include "../ui/windows/console_window.h"

class TextureInspectable : public Inspectable
{
public:
	void renderStaticContent(ImDrawList& drawList) override;
	void renderDynamicContent(ImDrawList& drawList) override;
};