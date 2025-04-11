#pragma once

#include <engine.h>
#include <rendering/gizmos/imgizmo.h>

#include "../project/project_manager.h"
#include "../pipelines/game_view_pipeline.h"
#include "../pipelines/scene_view_pipeline.h"
#include "../pipelines/preview_pipeline.h"

class ShadowDisk;
class ShadowMap;

class Model;
class LitMaterial;

enum class GameState {
	GAME_SLEEPING,
	GAME_LOADING,
	GAME_RUNNING,
	GAME_PAUSED,
};

namespace Runtime
{
	//
	// Base functions
	//

	int START_LOOP();
	int TERMINATE();

	//
	// Project
	//

	ProjectManager& projectManager();

	//
	// Game functions
	//

	void startGame();
	void stopGame();
	void pauseGame();
	void continueGame();

	GameState gameState();

	//
	// Pipelines getters
	//

	SceneViewPipeline& sceneViewPipeline();
	GameViewPipeline& gameViewPipeline();
	PreviewPipeline& previewPipeline();

	//
	// Gizmo getters
	//

	IMGizmo& sceneGizmos();

	//
	// Shadow getters
	//

	ShadowDisk* mainShadowDisk();
	ShadowMap* mainShadowMap();
};