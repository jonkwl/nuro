#pragma once

#include <engine.h>
#include <rendering/gizmos/imgizmo.h>

#include <filesystem>
namespace fs = std::filesystem;

#include "../project/project.h"
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

	void loadProject(const fs::path& path);
	const Project& getProject();

	//
	// Game functions
	//

	void startGame();
	void stopGame();
	void pauseGame();
	void continueGame();

	GameState getGameState();

	//
	// Pipelines getters
	//

	SceneViewPipeline& getSceneViewPipeline();
	GameViewPipeline& getGameViewPipeline();
	PreviewPipeline& getPreviewPipeline();

	//
	// Gizmo getters
	//

	IMGizmo& getSceneGizmos();

	//
	// Shadow getters
	//

	ShadowDisk* getMainShadowDisk();
	ShadowMap* getMainShadowMap();
};