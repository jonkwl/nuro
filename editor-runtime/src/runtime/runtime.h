#pragma once

#include "../core/rendering/gizmos/imgizmo.h"
#include "../core/physics/core/physics_context.h"

#include "../src/pipelines/game_view_pipeline.h"
#include "../src/pipelines/scene_view_pipeline.h"
#include "../src/pipelines/preview_pipeline.h"

class ShadowDisk;
class ShadowMap;

class Model;
class LitMaterial;

namespace Runtime
{
	// Base functions
	int START_LOOP(); // Main loop 
	int TERMINATE(); // Terminate and exit

	// Game functions
	void startGame();
	void stopGame();
	void pauseGame();
	void continueGame();
	bool gameRunning();
	bool gamePaused();

	// Pipelines getters
	SceneViewPipeline& getSceneViewPipeline();
	GameViewPipeline& getGameViewPipeline();
	PreviewPipeline& getPreviewPipeline();

	// Physics getters
	PhysicsContext& getGamePhysics();

	// Gizmo getters
	IMGizmo& getSceneGizmos();

	// Shadow getters
	ShadowDisk* getMainShadowDisk();
	ShadowMap* getMainShadowMap();

	// Other
	Model* getSphereModel();
	LitMaterial* getDefaultLit();
};