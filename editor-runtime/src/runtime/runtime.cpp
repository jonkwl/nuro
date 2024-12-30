#include "runtime.h"

#include <glad/glad.h>
#include <glm.hpp>

#include "../src/ui/editor_ui.h"
#include "../src/example/src/game_logic.h"

#include "../core/rendering/model/model.h"
#include "../core/rendering/shader/shader.h"
#include "../core/rendering/skybox/cubemap.h"
#include "../core/rendering/texture/texture.h"
#include "../core/rendering/shader/shader_pool.h"
#include "../core/rendering/shadows/shadow_map.h"
#include "../core/rendering/core/transformation.h"
#include "../core/rendering/shadows/shadow_disk.h"
#include "../core/rendering/primitives/global_quad.h"
#include "../core/rendering/material/unlit/unlit_material.h"

#include "../core/utils/log.h"
#include "../core/time/time.h"
#include "../core/input/input.h"
#include "../core/input/cursor.h"
#include "../core/physics/physics.h"
#include "../core/viewport/viewport.h"
#include "../core/ecs/ecs_collection.h"
#include "../core/transform/transform.h"
#include "../core/diagnostics/profiler.h"
#include "../core/diagnostics/diagnostics.h"
#include "../core/context/application_context.h"

namespace Runtime {

	// Pipelines
	SceneViewPipeline gSceneViewPipeline;
	GameViewPipeline gGameViewPipeline;

	// Physics
	PhysicsContext gGamePhysics;

	// Scene gizmos
	IMGizmo gSceneGizmos;

	// Shadow
	ShadowDisk* gMainShadowDisk = nullptr;
	ShadowMap* gMainShadowMap = nullptr;

	// Default assets
	Skybox gDefaultSkybox;

	// Game state management
	RegistryState gSceneState;
	bool gGameRunning = false;
	bool gGamePaused = false;

	// Default settings
	glm::ivec2 gStartupWindowSize = glm::ivec2(800.0f, 400.0f);

	//
	//
	// PRIVATE RUNTIME CORE METHODS
	//
	//

	void _loadAssets() {

		// Loading all shaders
		std::vector<std::string> shader_paths = {
			"../resources/shaders/materials",
			"../resources/shaders/postprocessing",
			"../resources/shaders/gizmo",
			"../resources/shaders/passes" };
		ShaderPool::loadAndCompile(shader_paths);

		// Create shadow disk
		uint32_t diskWindowSize = 4;
		uint32_t diskFilterSize = 8;
		uint32_t diskRadius = 5;
		gMainShadowDisk = new ShadowDisk(diskWindowSize, diskFilterSize, diskRadius);

		// Create default shadow map
		bool shadow_map_saved = false;
		gMainShadowMap = new ShadowMap(4096, 4096, 40.0f, 40.0f, 0.3f, 1000.0f);
		/*
		// Create default skybox
		Cubemap defaultCubemap = Cubemap::loadByCubemap("../resources/skybox/default/default_night.png");
		gDefaultSkybox = Skybox(defaultCubemap);

		// Set default skybox as current skybox
		gGameViewPipeline.linkSkybox(&gDefaultSkybox);
		*/
		// Load gizmo icons
		IconPool::loadAll("../resources/icons");
	}

	void _setupScripts() {

		// Create pipelines
		gSceneViewPipeline.create();
		gGameViewPipeline.create();

		// Create game physics instance
		gGamePhysics.create();

		// Setup scene gizmos
		gSceneGizmos.setup();

		// Create primitives
		GlobalQuad::create();

		// Setup input and cursor
		Input::setup();
		Cursor::setup();

		// Setup engine ui
		EditorUI::setup();

	}

	void _prepareFrame() {

		// Update diagnostics
		Diagnostics::step();

		// Update input system
		Input::update();

		// Clear frame color
		glClearColor(0.03f, 0.03f, 0.03f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

	}

	void _renderShadows()
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
		Profiler::start("shadow_pass");
		gMainShadowMap->render();
		Profiler::stop("shadow_pass");
	}

	void _renderEditor() {

		Profiler::start("ui_pass");

		EditorUI::newFrame();
		EditorUI::render();

		Profiler::stop("ui_pass");

	}

	void _stepGame() {

		// UPDATE GAME LOGIC
		update();

		// STEP GAME PHYSICS
		gGamePhysics.step(Time::deltaf());

		// SET SCENE VIEW PIPELINE TO UPDATED
		gSceneViewPipeline.setUpdated();

	}

	void _createLoadingScreen() {

		// Create viewport
		Viewport viewport;
		viewport.width = static_cast<float>(gStartupWindowSize.x);
		viewport.height = static_cast<float>(gStartupWindowSize.y);

		// Load model
		Model* model = Model::load("../resources/primitives/cube.fbx");
		Mesh& mesh = model->getMesh(0);
		
		// Get texture
		Texture texture = Texture::load("../resources/other/startup.jpg", TextureType::ALBEDO);

		// Load and compile internal shaders
		ShaderPool::loadAndCompile({ "../resources/shaders/startup" });
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
		glViewport(0, 0, viewport.width, viewport.height);

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
			Input::update();

			// Get delta
			float delta = Time::deltaf();

			// Cube rotation
			glm::vec2 mouseDelta = Input::mouseDelta() * delta;
			glm::vec3 axisX = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // X-axis rotation
			glm::vec3 axisY = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Y-axis rotation
			glm::quat rotationX = glm::angleAxis(mouseDelta.y, axisX); // Rotation around X axis (pitch)
			glm::quat rotationY = glm::angleAxis(mouseDelta.x, axisY); // Rotation around Y axis (yaw)
			glm::quat cubeRotation = rotationY * rotationX * cubeRotation;
			cubeRotation = glm::normalize(cubeRotation);
			transform.rotation = glm::slerp(transform.rotation, cubeRotation, 5.0f * delta);

			// Calculate transform matrices
			glm::mat4 model = Transformation::model(transform);
			glm::mat4 view = Transformation::view(glm::vec3(0.0f), glm::identity<glm::quat>());
			glm::mat4 projection = Transformation::projection(70.0f, 0.3f, 1000.0f, viewport);
			glm::mat4 mvp = projection * view * model;
			glm::mat4 normal = Transformation::normal(model);

			// Set shader uniforms
			shader->setMatrix4("mvpMatrix", mvp);
			shader->setMatrix3("normalMatrix", normal);
			shader->setVec4("baseColor", glm::vec4(1.0f));

			// Bind texture
			texture.bind(1);

			// Bind mesh
			glBindVertexArray(mesh.getVAO());

			// Render mesh
			glDrawElements(GL_TRIANGLES, mesh.getIndiceCount(), GL_UNSIGNED_INT, 0);

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

		// Set to fullscreen
		ApplicationContext::setFullscreen();

	}

	void _createApplicationContext() {

		// Create application context configuration
		ApplicationContext::Configuration config;
		config.api = API::OPENGL;
		config.windowSize = gStartupWindowSize;
		config.fullscreen = false;
		config.menubarVisible = false;
		config.vsync = false;

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
		
		// LOAD ASSETS, COMPILE SHADERS
		_loadAssets();

		// CALL ANY OTHER SCRIPTS NEEDING SETUP
		_setupScripts();

		// PERFORM GAMES SETUP LOGIC
		setup();

		// GENERATE ALL INITIAL QUEUES
		ECS::generateRenderQueue();

		// CREATE LOADING SCREEN
		// _createLoadingScreen();

		// TMP Set to fullscreen
		ApplicationContext::setFullscreen();

		while (ApplicationContext::running())
		{
			// START NEW APPLICATION CONTEXT FRAME
			ApplicationContext::startFrame();

			// UPDATE ANY SCRIPTS NEEDING UPDATE FOR NEXT FRAME (Time, Inputs etc.)
			_prepareFrame();

			// UPDATE GAME IF GAME IS RUNNING
			if (gGameRunning && !gGamePaused) _stepGame();

			// RENDER NEXT FRAME (full render pipeline pass)
			_renderShadows();
			gSceneViewPipeline.tryRender();
			gGameViewPipeline.tryRender();

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
		// Destroy all instances
		gSceneViewPipeline.destroy();
		gGameViewPipeline.destroy();
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
		awake();

		// Set game running state
		gGameRunning = true;
		gGamePaused = false;
	}

	void stopGame()
	{
		// Perform quit logic
		quit();

		// Set game running state
		gGameRunning = false;
		gGamePaused = false;

		// Restore scene state
		// ECS::loadState(gSceneState);
	}

	void pauseGame() {
		// Set game to paused
		gGamePaused = true;
	}

	void continueGame() {
		// Set game to not paused anymore
		gGamePaused = false;
	}

	bool gameRunning()
	{
		return gGameRunning;
	}

	bool gamePaused()
	{
		return gGamePaused;
	}

	SceneViewPipeline& getSceneViewPipeline()
	{
		return gSceneViewPipeline;
	}

	GameViewPipeline& getGameViewPipeline()
	{
		return gGameViewPipeline;
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