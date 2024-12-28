#pragma once

#include "window_includes.h"

#include "../core/rendering/postprocessing/post_processing.h"

class PostProcessingWindow : public EditorWindow
{
public:
	PostProcessingWindow(PostProcessing::Profile& targetProfile);

	void render();

	PostProcessing::Profile& getTargetProfile();
	void updateTargetProfile(PostProcessing::Profile& profile);

private:
	PostProcessing::Profile& targetProfile;
};
