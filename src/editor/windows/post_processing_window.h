#pragma once

#include "../src/editor/editor_window.h"
#include "../src/rendering/postprocessing/post_processing.h"

class PostProcessingWindow : public EditorWindow
{
public:
	PostProcessingWindow();
	void prepare();
private:
	PostProcessing::Configuration& configuration;
};
