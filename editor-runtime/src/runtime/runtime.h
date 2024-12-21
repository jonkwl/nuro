#pragma once

#include "../src/pipelines/scene_view_pipeline.h"
#include "../src/pipelines/game_view_pipeline.h"

class ShadowDisk;
class ShadowMap;

namespace Runtime
{
	// Base functions
	int32_t START_LOOP(); // Main loop 
	void TERMINATE(); // Terminate and exit

	// Game functions
	void startGame();
	void stopGame();
	bool gameRunning();

	// Pipelines getters
	SceneViewPipeline& getSceneViewPipeline();
	GameViewPipeline& getGameViewPipeline();

	// Shadow getters
	ShadowDisk* getMainShadowDisk();
	ShadowMap* getMainShadowMap();
};