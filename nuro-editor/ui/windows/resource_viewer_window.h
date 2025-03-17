#pragma once

#include <deque>

#include "editor_window.h"

class ResourceViewerWindow : public EditorWindow
{
public:
	ResourceViewerWindow();

	void render() override;
};
