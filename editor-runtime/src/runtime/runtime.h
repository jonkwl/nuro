#pragma once

#include <vector>
#include <glm.hpp>
#include <chrono>

#include <glad/glad.h> // tmp for some context variables

#include "../core/rendering/core/pre_pass.h"
#include "../core/rendering/core/forward_pass.h"
#include "../core/rendering/sceneview/scene_view_forward_pass.h"
#include "../core/rendering/ssao/ssao_pass.h"
#include "../core/rendering/postprocessing/post_processing_pipeline.h"
#include "../core/rendering/velocitybuffer/velocity_buffer.h"
#include "../core/rendering/skybox/skybox.h"
#include "../core/viewport/viewport.h"
#include "../core/rendering/gizmos/gizmos.h"

struct GLFWwindow;
class UnlitMaterial;
class Shader;
class Camera;
class ShadowDisk;
class ShadowMap;
class Entity;

class Runtime
{
public:
	static int START_LOOP(); // Main loop 
	static void TERMINATE(); // Terminate and exit

	// Entity system
	static Entity* createEntity(); // Links a given entity with the runtime (creating a runtime entity)
	static void destroyEntity(Entity* entity);

	static Camera& getCamera();

	// Context
	static GLFWwindow* glfw;
	static glm::vec2 windowSize;
	static bool fullscreen;

	// Render settings
	static glm::vec4 clearColor;	   // buffer clearing color
	static unsigned int msaaSamples;   // MSAA Samples
	static bool vsync;				   // V-Sync option
	static bool sceneViewWireframe;    // Scene view wireframe option
	static bool solidMode;			   // Solid mode option (simple render)
	static bool shadows;			   // Enable or disable rendering for all shadows
	static bool postProcessingEffects; // Enable or disable optional post processing effects
	static Skybox& currentSkybox;      // Current skybox to be rendered

	// Passes & Pipelines
	static PrePass prePass;
	static ForwardPass forwardPass;
	static SceneViewForwardPass sceneViewForwardPass;
	static SSAOPass ssaoPass;
	static VelocityBuffer velocityBuffer;
	static PostProcessingPipeline postProcessingPipeline;

	// Instances
	static QuickGizmo quickGizmo;

	// Perform resize for specific viewport
	static void resizeViewport(float width, float height);

private:
	static std::vector<Entity*> entityStack; // Stack of all entities that will be processed by the runtime
	static Viewport sceneViewport; // Scene viewport
	static Camera camera; // Main render camera
	static bool resized; // If window was resized
	static Skybox defaultSkybox; // Default skybox

	static ShadowDisk* mainShadowDisk; // Default shadow disk
	static ShadowMap* mainShadowMap;   // Default shadow map

private:
	static void setupGlfw(); // Setup glfw context
	static void setVSync(); // Set v sync option
	static void loadAssets(); // Load all defualt assets
	static void setupScripts(); // Call all scripts needing setup
	static void prepareFrame(); // Prepare next frame
	static void renderFrame(); // Render next game frame
	static void renderEditor(); // Render next editor frame
	static void finishFrame(); // Call to finalize current frame
	static void checkWindowResize(); // Perform resize if needed
	static void performResize(); // Perform resize

	// Context callbacks
	static void glfwErrorCallback(int error, const char* description);
	static void glfwWindowSizeCallback(GLFWwindow* window, int width, int height);
};