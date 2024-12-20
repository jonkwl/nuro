
#pragma once

#include <vector>
#include <glm.hpp>
#include <chrono>

#include <glad/glad.h> // tmp for some context variables

#include "../core/rendering/core/pre_pass.h"
#include "../core/rendering/core/forward_pass.h"
#include "../core/rendering/sceneview/scene_view_forward_pass.h"
#include "../core/rendering/ssao/ssao_pass.h"
#include "../core/rendering/postprocessing/post_processing.h"
#include "../core/rendering/postprocessing/post_processing_pipeline.h"
#include "../core/rendering/velocitybuffer/velocity_buffer.h"
#include "../core/rendering/skybox/skybox.h"
#include "../core/viewport/viewport.h"
#include "../core/rendering/gizmos/gizmos.h"

#include "../src/pipelines/scene_view_pipeline.h"
#include "../src/pipelines/game_view_pipeline.h"

struct GLFWwindow;
class UnlitMaterial;
class Shader;
class Camera;
class ShadowDisk;
class ShadowMap;

class Runtime
{
public:
	static int START_LOOP(); // Main loop 
	static void TERMINATE(); // Terminate and exit

	// Context
	static GLFWwindow* glfw;
	static glm::vec2 windowSize;
	static bool fullscreen;

	static SceneViewPipeline sceneViewPipeline;
	static GameViewPipeline gameViewPipeline;

	static ShadowDisk* mainShadowDisk; // Default shadow disk
	static ShadowMap* mainShadowMap;   // Default shadow map

private:
	static Skybox defaultSkybox; // Default skybox

private:
	// Render passes
	static void renderShadows(); // Renders all shadow passes

	// Methods
	static void setupGlfw(); // Setup glfw context
	static void loadAssets(); // Load all defualt assets
	static void setupScripts(); // Call all scripts needing setup
	static void prepareFrame(); // Prepare next frame
	static void renderEditor(); // Render next editor frame
	static void finishFrame(); // Call to finalize current frame

	// Context callbacks
	static void glfwErrorCallback(int error, const char* description);
};