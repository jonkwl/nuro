#pragma once

#include <vector>
#include <glm.hpp>
#include <chrono>

#include <glad/glad.h> // tmp for some context variables

#include "../src/rendering/core/pre_pass.h"
#include "../src/rendering/core/forward_pass.h"
#include "../src/rendering/ssao/ssao_pass.h"
#include "../src/rendering/postprocessing/post_processing_pipeline.h"
#include "../src/rendering/velocitybuffer/velocity_buffer.h"
#include "../src/rendering/skybox/skybox.h"
#include "../src/viewport/viewport.h"
#include "../src/rendering/gizmos/gizmos.h"

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
	// TO REMOVE
	// All default assets
	static Shader* prePassShader; // REMOVE, ADD TO MESH RENDERER ITSELF
	static Shader* shadowPassShader; // REMOVE, ADD TO MESH RENDERER ITSELF

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
	static GLenum cursorMode;
	static void setCursor(GLenum cursorMode);
	static void centerCursor();

	// Render settings
	static glm::vec4 clearColor;	   // buffer clearing color
	static unsigned int msaaSamples;   // MSAA Samples
	static bool vsync;				   // V-Sync option
	static bool wireframe;			   // Wireframe option
	static bool solidMode;			   // Solid mode option (simple render)
	static bool shadows;			   // Enable or disable rendering for all shadows
	static bool postProcessingEffects; // Enable or disable optional post processing effects
	static Skybox& currentSkybox;      // Current skybox to be rendered

	// Passes & Pipelines
	static PrePass prePass;
	static ForwardPass forwardPass;
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