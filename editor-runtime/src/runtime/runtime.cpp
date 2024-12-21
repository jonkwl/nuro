#include "runtime.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include <stb_image.h>
#include <stb_image_write.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/quaternion.hpp>

#include "../core/rendering/shader/shader.h"
#include "../core/rendering/shader/shader_pool.h"
#include "../core/rendering/material/unlit/unlit_material.h"
#include "../core/rendering/texture/texture.h"
#include "../core/rendering/model/model.h"
#include "../core/rendering/shadows/shadow_map.h"
#include "../core/rendering/shadows/shadow_disk.h"
#include "../core/rendering/primitives/quad.h"
#include "../core/utils/log.h"
#include "../core/diagnostics/profiler.h"
#include "../core/input/input.h"
#include "../core/rendering/core/transformation.h"
#include "../core/viewport/viewport.h"
#include "../core/rendering/skybox/cubemap.h"
#include "../core/time/time.h"
#include "../core/diagnostics/diagnostics.h"
#include "../core/input/cursor.h"

#include "../src/ui/editor_ui.h"
#include "../src/example/src/game_logic.h"

namespace Runtime {

	GLFWwindow* _window = nullptr;
	glm::vec2 _windowSize = glm::vec2(0.0f);
	bool _fullscreen = true;

	SceneViewPipeline _sceneViewPipeline;
	GameViewPipeline _gameViewPipeline;

	ShadowDisk* _mainShadowDisk = nullptr;
	ShadowMap* _mainShadowMap = nullptr;

	Skybox _defaultSkybox;

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

		// Check for _fullscreen
		if (_fullscreen)
		{
			GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

			_windowSize = glm::vec2(mode->width, mode->height);
		}

		// Create window
		_window = glfwCreateWindow(_windowSize.x, _windowSize.y, "Rendering Alpha", nullptr, nullptr);

		if (_window == nullptr)
		{
			Log::printError("GLFW", "Creation of window failed");
		}

		// Load graphics api
		glfwMakeContextCurrent(_window);
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
		_mainShadowDisk = new ShadowDisk(diskWindowSize, diskFilterSize, diskRadius);

		// Create default shadow map
		bool shadow_map_saved = false;
		_mainShadowMap = new ShadowMap(4096, 4096, 40.0f, 40.0f, 0.3f, 1000.0f);

		// Create default skybox
		Cubemap defaultCubemap = Cubemap::loadByCubemap("../resources/skybox/default/default_night.png");
		_defaultSkybox = Skybox(defaultCubemap);

		// Set default skybox as current skybox
		_gameViewPipeline.setSkybox(&_defaultSkybox);

		// Load gizmo icons
		GizmoIconPool::loadAll("../resources/gizmos");
	}

	void _setupScripts() {

		// Set context for scripts needing window context
		Input::setContext(_window);
		Cursor::setContext(_window);

		// Setup pipelines
		_sceneViewPipeline.setup();
		_gameViewPipeline.setup();

		// Create primitives
		Quad::create();

		// Setup engine ui
		EditorUI::setup(_window);

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
		_mainShadowMap->render();
		Profiler::stop("shadow_pass");
	}

	void _renderEditor() {

		Profiler::start("ui_pass");

		EditorUI::newFrame();
		EditorUI::render();

		Profiler::stop("ui_pass");

	}

	void _finishFrame() {

		// Sqap _window buffers and poll events
		glfwSwapBuffers(_window);
		glfwPollEvents();

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

		while (!glfwWindowShouldClose(_window))
		{
			// UPDATE ANY SCRIPTS NEEDING UPDATE FOR NEXT FRAME (Time, Inputs etc.)
			_prepareFrame();

			// UPDATE GAME LOGIC
			update();

			//
			// EXTERNAL TRANSFORM MANIPULATION HERE (e.g. physics)
			//

			// RENDER NEXT FRAME (full render pipeline pass)
			_renderShadows();
			_sceneViewPipeline.render();
			_gameViewPipeline.render();

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
		if (_window != nullptr)
		{
			glfwDestroyWindow(_window);
			glfwTerminate();
		}
		std::exit(0);
	}

	SceneViewPipeline& getSceneViewPipeline()
	{
		return _sceneViewPipeline;
	}

	GameViewPipeline& getGameViewPipeline()
	{
		return _gameViewPipeline;
	}

	ShadowDisk* getMainShadowDisk()
	{
		return _mainShadowDisk;
	}

	ShadowMap* getMainShadowMap()
	{
		return _mainShadowMap;
	}

}