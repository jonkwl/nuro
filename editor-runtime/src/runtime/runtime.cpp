#include "runtime.h"

#include <glad/glad.h>
#include <glm.hpp>

#include <thread>
#include <chrono>

#include "../src/ui/editor_ui.h"
#include "../src/example/src/game_logic.h"

#include "../core/rendering/model/model.h"
#include "../core/rendering/shader/shader.h"
#include "../core/rendering/skybox/cubemap.h"
#include "../core/rendering/texture/texture.h"
#include "../core/rendering/shader/shader_pool.h"
#include "../core/rendering/shadows/shadow_map.h"
#include "../core/rendering/shadows/shadow_disk.h"
#include "../core/rendering/material/lit/lit_material.h"
#include "../core/rendering/transformation/transformation.h"

#include "../core/utils/console.h"
#include "../core/time/time.h"
#include "../core/physics/physics.h"
#include "../core/viewport/viewport.h"
#include "../core/ecs/ecs_collection.h"
#include "../core/transform/transform.h"
#include "../core/diagnostics/profiler.h"
#include "../core/context/application_context.h"

namespace Runtime {

	// Pipelines
	SceneViewPipeline gSceneViewPipeline;
	GameViewPipeline gGameViewPipeline;
	PreviewPipeline gPreviewPipeline;

	// Physics
	PhysicsContext gGamePhysics;

	// Scene gizmos
	IMGizmo gSceneGizmos;

	// Shadow
	ShadowDisk* gMainShadowDisk = nullptr;
	ShadowMap* gMainShadowMap = nullptr;

	// Default assets
	Skybox gDefaultSkybox;

	// Global game state
	GameState gGameState = GameState::GAME_SLEEPING;

	// Global registry state
	RegistryState gSceneState;

	// Default settings
	glm::ivec2 gStartupWindowSize = glm::ivec2(800.0f, 400.0f);

	//
	//
	// PRIVATE RUNTIME CORE METHODS
	//
	//

	void _loadDependencies() {
		AssetLoader& loader = ApplicationContext::getAssetLoader();

		// Loading all shaders
		std::vector<std::string> shader_paths = {
			"../resources/shaders/materials",
			"../resources/shaders/postprocessing",
			"../resources/shaders/gizmo",
			"../resources/shaders/passes" };
		ShaderPool::load(shader_paths);

		// Create default texture
		// Default texture should exist for the whole application lifetime, so just forget about the pointer (for now)
		Texture* defaultTexture = new Texture();
		defaultTexture->setSource(TextureType::IMAGE_RGBA, "../resources/icons/default_texture.png");
		loader.createSync(defaultTexture);
		Texture::setDefaultTexture(defaultTexture);

		// Load various editor icons
		IconPool::createFallback("../resources/icons/invalid.png");
		IconPool::load("../resources/icons");

		// Link default skybox (can be nullptr)
		gGameViewPipeline.linkSkybox(&gDefaultSkybox);
	}

	void _createResources() {

		// Create pipelines
		gSceneViewPipeline.create();
		gGameViewPipeline.create();
		gPreviewPipeline.create();

		// Create game physics instance
		gGamePhysics.create();

		// Setup scene gizmos
		gSceneGizmos.create();

		// TMP:
		// Create main shadow disk and main shadow map
		uint32_t diskWindowSize = 4;
		uint32_t diskFilterSize = 8;
		uint32_t diskRadius = 5;
		gMainShadowDisk = new ShadowDisk(diskWindowSize, diskFilterSize, diskRadius);
		gMainShadowMap = new ShadowMap(4096, 4096);
		gMainShadowMap->create();

	}

	void _renderShadowsGlobal()
	{
		// WARNING
		// 
		// Right now, the shadow passes will use the "current model matrix" from the mesh renderer.
		// As the shadows will be rendered before the other rendering pipelines,
		// which prepare the mesh renderer by among other things calculating and 
		// settings its "current model matrix", the "current model matrix" will always be
		// one frame delayed, ultimately delaying object movement for shadows by one frame
		//
		// Fix: Calculate transform matrices before rendering shadows

		//
		// SHADOW PASS
		// Render shadow map
		//

		// Make sure shadow map is created
		if (!gMainShadowMap) return;

		// Temporary: Render first spotlight to be found in registry
		auto spotlights = ECS::gRegistry.view<TransformComponent, SpotlightComponent>();
		for (auto [entity, transform, spotlight] : spotlights.each()) {
			Profiler::start("shadow_pass");
			gMainShadowMap->castShadows(spotlight, transform);
			Profiler::stop("shadow_pass");
			break;
		}
	}

	void _renderEditor() {

		Profiler::start("ui_pass");

		EditorUI::newFrame();
		EditorUI::render();

		Profiler::stop("ui_pass");

	}

	void _stepGame() {

		// UPDATE GAME LOGIC
		gameUpdate();

		// STEP GAME PHYSICS
		gGamePhysics.step(Time::deltaf());

	}

	void _createApplicationContext() {

		// Create application context configuration
		ApplicationContext::Configuration config;
		config.api = API::OPENGL;
		config.windowSize = gStartupWindowSize;
		config.fullscreen = false;
		config.menubarVisible = false;
		config.vsync = true;
		config.resizeable = false;

		// Create application context instance
		ApplicationContext::create(config);

	}

	//
	//
	// RUNTIME BASE METHODS (START & TERMINATE)
	//
	//

	int START_LOOP()
	{
		// CREATE CONTEXT
		_createApplicationContext();

		// LOAD DEPENDENCIES (SHADERS ETC)
		_loadDependencies();

		// CREATE RESOURCES (RENDER PASSES, PHYSICS CONTEXT ETC)
		_createResources();

		// GAME SETUP
		gameSetup();
		ECS::generateRenderQueue();

		// LAUNCH EDITOR
		Console::out::welcome();
		EditorUI::setup();
		ApplicationContext::setResizeable(true);
		ApplicationContext::maximizeWindow();

		while (ApplicationContext::running())
		{
			// START NEW APPLICATION CONTEXT FRAME
			ApplicationContext::nextFrame();

			// CLEAR FRAME COLOR
			glClearColor(0.03f, 0.03f, 0.03f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// UPDATE GAME IF GAME IS RUNNING
			if (gGameState == GameState::GAME_RUNNING) _stepGame();

			// RENDER NEXT FRAME
			_renderShadowsGlobal();
			gSceneViewPipeline.render();
			gGameViewPipeline.render();
			gPreviewPipeline.render();

			// RENDER EDITOR
			_renderEditor();

			// END CURRENT FRAME
			ApplicationContext::endFrame();
		}

		// Exit application

		return TERMINATE();
	}

	int TERMINATE()
	{
		// Destroy all pipelines
		gSceneViewPipeline.destroy();
		gGameViewPipeline.destroy();
		gPreviewPipeline.destroy();

		// Destroy physics
		gGamePhysics.destroy();

		// Destroy context
		ApplicationContext::destroy();

		// Exit application
		std::exit(0);

		return 0;
	}

	void startGame()
	{
		// Re-Generate render queue

		// Cache scene state
		// gSceneState = ECS::captureState();

		// Perform awake logic
		gameAwake();

		// Set game running state
		gGameState = GameState::GAME_RUNNING;
	}

	void stopGame()
	{
		// Perform quit logic
		gameQuit();

		// Set game running state
		gGameState = GameState::GAME_SLEEPING;

		// Restore scene state
		// ECS::loadState(gSceneState);
	}

	void pauseGame() {
		if (gGameState == GameState::GAME_RUNNING) {
			gGameState = GameState::GAME_PAUSED;
		}
	}

	void continueGame() {
		gGameState = GameState::GAME_RUNNING;
	}

	GameState getGameState() {
		return gGameState;
	}

	SceneViewPipeline& getSceneViewPipeline()
	{
		return gSceneViewPipeline;
	}

	GameViewPipeline& getGameViewPipeline()
	{
		return gGameViewPipeline;
	}

	PreviewPipeline& getPreviewPipeline()
	{
		return gPreviewPipeline;
	}

	PhysicsContext& getGamePhysics()
	{
		return gGamePhysics;
	}

	IMGizmo& getSceneGizmos()
	{
		return gSceneGizmos;
	}

	PhysicsContext& getPhysicsContext()
	{
		return gGamePhysics;
	}

	ShadowDisk* getMainShadowDisk()
	{
		return gMainShadowDisk;
	}

	ShadowMap* getMainShadowMap()
	{
		return gMainShadowMap;
	}

}