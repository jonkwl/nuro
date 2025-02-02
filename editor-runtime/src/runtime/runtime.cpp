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
		// Loading all shaders
		std::vector<std::string> shader_paths = {
			"../resources/shaders/materials",
			"../resources/shaders/postprocessing",
			"../resources/shaders/gizmo",
			"../resources/shaders/passes" };
		ShaderPool::load(shader_paths);

		// Create shadow disk
		uint32_t diskWindowSize = 4;
		uint32_t diskFilterSize = 8;
		uint32_t diskRadius = 5;
		gMainShadowDisk = new ShadowDisk(diskWindowSize, diskFilterSize, diskRadius);

		// Create default spotlight shadow map
		gMainShadowMap = new ShadowMap(4096, 4096);
		gMainShadowMap->create();

		// Load gizmo icons
		IconPool::load("../resources/icons");

		// Create default skybox
		/*
		Cubemap defaultCubemap = Cubemap::loadByCubemap("../resources/skybox/default/default_night.png");
		gDefaultSkybox = Skybox(defaultCubemap);

		// Set default skybox as current skybox
		gGameViewPipeline.linkSkybox(&gDefaultSkybox);
		*/
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

	void _createLoadingScreen() {

		// Create viewport
		Viewport viewport(static_cast<float>(gStartupWindowSize.x), static_cast<float>(gStartupWindowSize.y));

		// Load model
		Model* model = Model::load("../resources/primitives/cube.fbx");
		const Mesh* mesh = model->getMesh(0);
		
		// Get texture
		Texture texture = Texture::load("../resources/other/startup.jpg", TextureType::ALBEDO);

		// Load and compile internal shaders
		ShaderPool::load({ "../resources/shaders/startup" });
		Shader* shader = ShaderPool::get("startup_model");
		shader->bind();
		shader->setInt("colorTexture", 1);

		// Initialize times
		float duration = 20.0f;
		float passed = 0.0f;

		// Create transform
		TransformComponent transform;
		transform.position = glm::vec3(0.0f, 0.0f, 5.0f);

		// Update viewport
		glViewport(0, 0, viewport.getWidth_gl(), viewport.getHeight_gl());

		// Set culling to back face
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		// Enable depth testing
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		while(passed < duration){

			// Start new frame
			ApplicationContext::startFrame();

			// Clear color buffer
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Update inputs
			// Input::update();

			// Get delta
			float delta = Time::deltaf();

			// Cube rotation
			// glm::vec2 mouseDelta = Input::mouseDelta() * delta;
			glm::vec2 mouseDelta = glm::vec2(0.0f, 0.0f);
			glm::vec3 axisX = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // X-axis rotation
			glm::vec3 axisY = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Y-axis rotation
			glm::quat rotationX = glm::angleAxis(mouseDelta.y, axisX); // Rotation around X axis (pitch)
			glm::quat rotationY = glm::angleAxis(mouseDelta.x, axisY); // Rotation around Y axis (yaw)
			glm::quat cubeRotation = rotationY * rotationX * cubeRotation;
			cubeRotation = glm::normalize(cubeRotation);
			transform.rotation = glm::slerp(transform.rotation, cubeRotation, 5.0f * delta);

			// Calculate transform matrices
			glm::mat4 model = Transformation::model(transform.position, transform.rotation, transform.scale);
			glm::mat4 view = Transformation::view(glm::vec3(0.0f), glm::identity<glm::quat>());
			glm::mat4 projection = Transformation::projection(70.0f, viewport.getAspect(), 0.3f, 1000.0f);
			glm::mat4 mvp = projection * view * model;
			glm::mat4 normal = Transformation::normal(model);

			// Set shader uniforms
			shader->setMatrix4("mvpMatrix", mvp);
			shader->setMatrix3("normalMatrix", normal);
			shader->setVec4("baseColor", glm::vec4(1.0f));

			// Bind texture
			texture.bind(1);

			// Bind mesh
			glBindVertexArray(mesh->getVAO());

			// Render mesh
			glDrawElements(GL_TRIANGLES, mesh->getIndiceCount(), GL_UNSIGNED_INT, 0);

			// End frame
			ApplicationContext::endFrame();

			// Update passed time
			passed += delta;

		}

		// Clear frame
		ApplicationContext::startFrame();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		ApplicationContext::endFrame();

		// Delete model
		delete model;

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

		// LAUNCH EDITOR
		Console::out::welcome();
		EditorUI::setup();
		ApplicationContext::maximizeWindow();
		ApplicationContext::setResizeable(true);

		// PERFORM ASYNC GAME SETUP (TMP / TESTING)
		std::jthread setup([]() {
			gameSetup();
			ECS::generateRenderQueue();
		});

		while (ApplicationContext::running())
		{
			// START NEW APPLICATION CONTEXT FRAME
			ApplicationContext::startFrame();

			// CLEAR FRAME COLOR
			glClearColor(0.03f, 0.03f, 0.03f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// UPDATE GAME IF GAME IS RUNNING
			if (gGameState == GameState::GAME_RUNNING) {
				_stepGame();
			}

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