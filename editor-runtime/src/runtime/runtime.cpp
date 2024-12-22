#include "runtime.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <PxPhysicsAPI.h>

#include "../src/example/src/game_logic.h"
#include "../src/ui/editor_ui.h"

#include "../core/rendering/material/unlit/unlit_material.h"
#include "../core/rendering/shadows/shadow_disk.h"
#include "../core/rendering/shadows/shadow_map.h"
#include "../core/rendering/core/transformation.h"
#include "../core/rendering/shader/shader_pool.h"
#include "../core/rendering/texture/texture.h"
#include "../core/rendering/shader/shader.h"
#include "../core/rendering/primitives/quad.h"
#include "../core/rendering/skybox/cubemap.h"
#include "../core/rendering/model/model.h"

#include "../core/diagnostics/diagnostics.h"
#include "../core/diagnostics/profiler.h"
#include "../core/ecs/ecs_collection.h"
#include "../core/viewport/viewport.h"
#include "../core/input/cursor.h"
#include "../core/input/input.h"
#include "../core/time/time.h"
#include "../core/utils/log.h"


namespace Runtime {

	// Window context variables
	GLFWwindow* gWindow = nullptr;
	glm::vec2 gWindowSize = glm::vec2(0.0f);
	bool gFullscreen = true;

	// Pipelines
	SceneViewPipeline gSceneViewPipeline;
	GameViewPipeline gGameViewPipeline;

	// Physics
	PhysicsInstance gPhysicsInstance;

	// Shadow
	ShadowDisk* gMainShadowDisk = nullptr;
	ShadowMap* gMainShadowMap = nullptr;

	// Default assets
	Skybox gDefaultSkybox;

	// Game state management
	RegistryState gSceneState;
	bool gGameRunning = false;

	//
	//
	// PRIVATE RUNTIME CORE METHODS
	//
	//

	void _glfwErrorCallback(int32_t error, const char* description)
	{

		Log::printError("GLFW", "Error: " + std::to_string(error), description);

	}

	void _setupGlfw() {

		// Set error callback and initialize context
		glfwSetErrorCallback(_glfwErrorCallback);
		glfwInit();

		// Set versions
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Enable HDR output
		glfwWindowHint(GLFW_RED_BITS, 10);
		glfwWindowHint(GLFW_GREEN_BITS, 10);
		glfwWindowHint(GLFW_BLUE_BITS, 10);
		glfwWindowHint(GLFW_ALPHA_BITS, 2);

		// Check for gFullscreen
		if (gFullscreen)
		{
			GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

			gWindowSize = glm::vec2(mode->width, mode->height);
		}

		// Create window
		gWindow = glfwCreateWindow(gWindowSize.x, gWindowSize.y, "Rendering Alpha", nullptr, nullptr);

		if (gWindow == nullptr)
		{
			Log::printError("GLFW", "Creation of window failed");
		}

		// Load graphics api
		glfwMakeContextCurrent(gWindow);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			Log::printError("GLFW", "Initialization of GLAD failed");
		}

		// Debug graphics api version
		const char* version = (const char*)glGetString(GL_VERSION);
		Log::printProcessDone("GLFW", "Initialized, OpenGL version: " + std::string(version));

		// Disable vsync
		glfwSwapInterval(0);
	}

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

		// Create default skybox
		Cubemap defaultCubemap = Cubemap::loadByCubemap("../resources/skybox/default/default_night.png");
		gDefaultSkybox = Skybox(defaultCubemap);

		// Set default skybox as current skybox
		gGameViewPipeline.setSkybox(&gDefaultSkybox);

		// Load gizmo icons
		GizmoIconPool::loadAll("../resources/gizmos");
	}

	void _setupScripts() {

		// Set context for scripts needing window context
		Input::setContext(gWindow);
		Cursor::setContext(gWindow);

		// Setup pipelines
		gSceneViewPipeline.setup();
		gGameViewPipeline.setup();

		// Create physics instance
		gPhysicsInstance.create();

		// Create primitives
		Quad::create();

		// Setup engine ui
		EditorUI::setup(gWindow);

	}

	void _prepareFrame() {

		// Update time
		Time::step(glfwGetTime());

		// Update diagnostics
		Diagnostics::step();

		// Update input system
		Input::step();

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

	void _finishFrame() {

		// Swap gWindow buffers and poll events
		glfwSwapBuffers(gWindow);
		glfwPollEvents();

	}

	void _stepGame() {

		// UPDATE GAME LOGIC
		update();

		// STEP PHYSICS
		gPhysicsInstance.step(Time::deltaf());

		// SET SCENE VIEW PIPELINE TO UPDATED
		gSceneViewPipeline.setUpdated();

	}

	//
	//
	// RUNTIME BASE METHODS (START & TERMINATE)
	//
	//

	int32_t START_LOOP()
	{
		// CREATE CONTEXT AND LOAD GRAPHICS API //
		Log::printProcessStart("Runtime", "Creating context...");
		_setupGlfw(); // Setup window
		Log::printProcessDone("Runtime", "Context created");

		// LOAD ASSETS, COMPILE SHADERS
		_loadAssets();

		// CALL ANY OTHER SCRIPTS NEEDING SETUP
		_setupScripts();

		// PERFORM GAMES SETUP LOGIC
		setup();

		while (!glfwWindowShouldClose(gWindow))
		{
			// UPDATE ANY SCRIPTS NEEDING UPDATE FOR NEXT FRAME (Time, Inputs etc.)
			_prepareFrame();

			// UPDATE GAME IF GAME IS RUNNING
			if (gGameRunning) _stepGame();

			// RENDER NEXT FRAME (full render pipeline pass)
			_renderShadows();
			gSceneViewPipeline.tryRender();
			gGameViewPipeline.tryRender();

			// RENDER EDITOR
			_renderEditor();

			// FINISH CURRENT FRAME
			_finishFrame();
		}

		// Exit application

		glfwTerminate();
		return 0;
	}

	void TERMINATE()
	{
		// Should destroy pipelines here
		gPhysicsInstance.destroy();

		if (gWindow != nullptr)
		{
			glfwDestroyWindow(gWindow);
			glfwTerminate();
		}

		std::exit(0);
	}

	void startGame()
	{
		// Cache scene state
		// gSceneState = ECS::captureState();

		// Perform awake logic
		awake();

		// Set game running state
		gGameRunning = true;
	}

	void stopGame()
	{
		// Perform quit logic
		quit();

		// Set game running state
		gGameRunning = false;

		// Restore scene state
		// ECS::loadState(gSceneState);
	}

	bool gameRunning()
	{
		return gGameRunning;
	}

	SceneViewPipeline& getSceneViewPipeline()
	{
		return gSceneViewPipeline;
	}

	GameViewPipeline& getGameViewPipeline()
	{
		return gGameViewPipeline;
	}

	PhysicsInstance& getPhysicsInstance()
	{
		return gPhysicsInstance;
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