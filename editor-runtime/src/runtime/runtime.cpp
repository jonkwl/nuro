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
	Texture* gDefaultTexture = new Texture();
	Cubemap* gDefaultCubemap = new Cubemap();
	Skybox gDefaultSkybox;

	// Global game state
	GameState gGameState = GameState::GAME_SLEEPING;

	// Default settings
	glm::ivec2 gStartupWindowSize = glm::ivec2(800.0f, 400.0f);

	//
	//
	// PRIVATE RUNTIME CORE METHODS
	//
	//

	void _loadDependencies() {
		ResourceLoader& loader = ApplicationContext::getResourceLoader();

		// Load shaders
		ShaderPool::loadAllSync("../resources/shaders/materials");
		ShaderPool::loadAllSync("../resources/shaders/postprocessing");
		ShaderPool::loadAllSync("../resources/shaders/gizmo");
		ShaderPool::loadAllSync("../resources/shaders/passes");

		// Create default texture
		gDefaultTexture->setSource(TextureType::IMAGE_RGBA, "../resources/icons/default_texture.png");
		loader.createSync(gDefaultTexture);
		Texture::setDefaultTexture(gDefaultTexture->getId());

		// Load various editor icons
		IconPool::createFallbackIconSync("../resources/icons/invalid.png");
		IconPool::loadAllSync("../resources/icons");

		// Create default cubemap
		gDefaultCubemap->setSource_Cross("../resources/skybox/default/default_night.png");
		// loader.createAsync(gDefaultCubemap);

		// Create default skybox
		gDefaultSkybox.setCubemap(gDefaultCubemap);
		gDefaultSkybox.create();
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
		config.visible = false;

		// Create application context instance
		ApplicationContext::create(config);

	}

	void _launchEditor() {
		// Print welcome
		Console::out::welcome();

		// Setup editor ui
		EditorUI::setup();

		// Show editor window
		ApplicationContext::setResizeable(true);
		ApplicationContext::maximizeWindow();
		ApplicationContext::setVisible(true);
	}

	void _nextFrame() {
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
		Profiler::start("ui_pass");
		EditorUI::newFrame();
		EditorUI::render();
		Profiler::stop("ui_pass");

		// END CURRENT FRAME
		ApplicationContext::endFrame();
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

		// LAUNCH EDITOR
		_launchEditor();

		// GAME SETUP
		gameSetup();
		ECS::generateRenderQueue();

		// TMP
		ApplicationContext::getResourceLoader().createAsync(gDefaultCubemap);

		while (ApplicationContext::running())
		{
			_nextFrame();
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
		// Re-generate render queue
		// ...

		// Cache scene state
		// ...

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
		// ...
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