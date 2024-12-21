#pragma once

#include <vector>
#include <glm.hpp>
#include <chrono>

#include "../src/pipelines/scene_view_pipeline.h"
#include "../src/pipelines/game_view_pipeline.h"

class ShadowDisk;
class ShadowMap;

namespace Runtime
{
	// Base functions
	int32_t START_LOOP(); // Main loop 
	void TERMINATE(); // Terminate and exit

	// Pipelines getters
	SceneViewPipeline& getSceneViewPipeline();
	GameViewPipeline& getGameViewPipeline();

	// Shadow getters
	ShadowDisk* getMainShadowDisk();
	ShadowMap* getMainShadowMap();
};