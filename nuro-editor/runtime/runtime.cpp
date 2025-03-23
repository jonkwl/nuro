#include "runtime.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <thread>
#include <chrono>

#include "../ui/editor_ui.h"
#include "../testing/game_logic.h"
#include "../ui/windows/console_window.h"
#include "../reflection/component_registry.h"
#include "../ui/windows/insight_panel_window.h"
#include "../ui/inspectables/welcome_inspectable.h"

#include <time/time.h>
#include <utils/console.h>
#include <physics/physics.h>
#include <viewport/viewport.h>
#include <ecs/ecs_collection.h>
#include <transform/transform.h>
#include <diagnostics/profiler.h>
#include <context/application_context.h>

#include <rendering/model/model.h>
#include <rendering/shader/shader.h>
#include <rendering/skybox/cubemap.h>
#include <rendering/texture/texture.h>
#include <rendering/shader/shader_pool.h>
#include <rendering/shadows/shadow_map.h>
#include <rendering/shadows/shadow_disk.h>
#include <rendering/material/lit/lit_material.h>
#include <rendering/transformation/transformation.h>

namespace Runtime {

	// Project
	Project gProject;

	// Pipelines
	SceneViewPipeline gSceneViewPipeline;
	GameViewPipeline gGameViewPipeline;
	PreviewPipeline gPreviewPipeline;

	// Scene gizmos
	IMGizmo gSceneGizmos;

	// Shadow
	ShadowDisk* gMainShadowDisk = nullptr;
	ShadowMap* gMainShadowMap = nullptr;

	// Default assets
	Skybox gDefaultSkybox;
	uint32_t gDefaultCubemap = 0;

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
		Console::out::start("Runtime", "Loading dependencies");

		ResourceManager& resource = ApplicationContext::resourceManager();

		// Load shaders
		ShaderPool::loadAllSync("./shaders/materials");
		ShaderPool::loadAllSync("./shaders/postprocessing");
		ShaderPool::loadAllSync("./shaders/gizmo");
		ShaderPool::loadAllSync("./shaders/passes");

		// Create default texture
		auto [defaultTextureId, defaultTexture] = resource.create<Texture>("default-texture");
		defaultTexture->setSource(TextureType::IMAGE_RGBA, "./resources/icons/fallback/fallback_texture.png");
		resource.loadSync(defaultTexture);
		Texture::setDefaultTexture(defaultTexture);

		// Load various editor icons
		IconPool::createFallbackIcon("./resources/icons/fallback/fallback_icon.png");
		IconPool::loadAllSync("./resources/icons/shared");
		IconPool::loadAllAsync("./resources/icons/assets");
		IconPool::loadAllAsync("./resources/icons/components");
		IconPool::loadAllAsync("./resources/icons/scene");
		IconPool::loadAllAsync("./resources/icons/post-processing");

		// Create default cubemap
		auto [cubemapId, cubemap] = resource.create<Cubemap>("default-cubemap");
		gDefaultCubemap = cubemapId;
		cubemap->setSource_Cross("./resources/skybox/default/default_night.png");

		// Create default skybox
		gDefaultSkybox.setCubemap(cubemap);
		gDefaultSkybox.create();
		gGameViewPipeline.linkSkybox(&gDefaultSkybox);

		Console::out::done("Runtime", "Loaded dependencies");
	}

	void _createResources() {
		Console::out::start("Runtime", "Creating resources");

		// Create pipelines
		gSceneViewPipeline.create();
		gGameViewPipeline.create();
		gPreviewPipeline.create();

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

		Console::out::done("Runtime", "Created resources");

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
		auto spotlights = ECS::main().view<TransformComponent, SpotlightComponent>();
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
		ApplicationContext::physicsContext().step();

	}

	void _initiateConsole() {
		// Space
		Console::print >> Console::endl;

		// Register callback to log engine outputs to the editor application context
		Console::out::logEvent().subscribe(
			[](const std::string& origin, const std::string& content, LogType logType)
			{
				if (logType != LogType::DEFAULT)
					ConsoleWindow::log(ConsoleLog(origin, content, logType));
			}
		);
	}

	void _createApplicationContext() {

		_initiateConsole();

		// Create application context configuration
		ApplicationContext::Configuration config;
		config.api = API::OPENGL;
		config.windowSize = gStartupWindowSize;
		config.fullscreen = false;
		config.menubarVisible = false;
		config.vsync = false;
		config.resizeable = false;
		config.visible = false;

		// Create application context instance
		ApplicationContext::create(config);

	}

	void _launchEditor() {
		// Print welcome
		Console::out::welcome();

		// Create editor component registry
		ComponentRegistry::create();

		// Setup editor ui
		EditorUI::setup();

		// Show editor window
		ApplicationContext::setResizeable(true);
		ApplicationContext::maximizeWindow();
		ApplicationContext::setVisible(true);

		// Show welcome inspectable
		InsightPanelWindow::inspect<WelcomeInspectable>();
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
		// gPreviewPipeline.render();

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

		// SETUP GAME
		gameSetup();

		// LOAD DEFAULT CUBEMAP
		ResourceManager& resource = ApplicationContext::resourceManager();
		resource.loadAsync(resource.get<Cubemap>(gDefaultCubemap));

		// MAIN LOOP
		while (ApplicationContext::running()) _nextFrame();

		// EXIT APPLICATION
		return TERMINATE();
	}

	int TERMINATE()
	{
		// Destroy all pipelines
		gSceneViewPipeline.destroy();
		gGameViewPipeline.destroy();
		gPreviewPipeline.destroy();

		// Destroy context
		ApplicationContext::destroy();

		// Exit application
		std::exit(0);

		return 0;
	}

	void loadProject(const fs::path& path)
	{
		gProject.load(path);
	}

	const Project& getProject()
	{
		return gProject;
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

	IMGizmo& getSceneGizmos()
	{
		return gSceneGizmos;
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