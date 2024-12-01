#pragma once

#include <vector>
#include <glm.hpp>
#include <chrono>

#include <glad/glad.h> // tmp for some context variables

#include "../src/rendering/core/pre_pass.h"
#include "../src/rendering/core/forward_pass.h"
#include "../src/rendering/ssao/ssao_pass.h"
#include "../src/rendering/postprocessing/post_processing_pipeline.h"
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
	static int START_LOOP(); // Main loop 
	static void TERMINATE(); // Terminate and exit

	// Entity system
	static Entity* createEntity(); // Links a given entity with the runtime (creating a runtime entity)
	static void destroyEntity(Entity* entity);

	static Camera& getCamera();

	// All default assets
	static UnlitMaterial* defaultMaterial; // MOVE TO MESH RENDERER

	static Skybox defaultSkybox; // MOVE TO FORWARD PASS

	// Default shaders
	static Shader* prePassShader; // REMOVE, ADD TO MESH RENDERER ITSELF
	static Shader* shadowPassShader; // REMOVE, ADD TO MESH RENDERER ITSELF
	static Shader* velocityPassShader; // REMOVE, ADD TO MESH RENDERER ITSELF

	// Skybox settings
	// MOVE TO FORWARD PASS
	static bool skyboxEnabled;
	static Skybox& selectedSkybox; // Currently selected skybox

	// Context
	static GLFWwindow* glfw;
	static glm::vec2 windowSize;
	static bool fullscreen;
	static GLenum cursorMode;
	static void setCursor(GLenum cursorMode);
	static void centerCursor();

	// Viewports
	static Viewport sceneViewport;

	// Render settings
	// CHECK FOR DEPENDENCIES FROM OTHER SCRIPTS, SHOULD STAY
	static glm::vec4 clearColor;	   // buffer clearing color
	static unsigned int msaaSamples;   // MSAA Samples
	static bool vsync;				   // V-Sync option
	static bool wireframe;			   // Wireframe option
	static bool solidMode;			   // Solid mode option (simple render)
	static bool shadows;			   // Enable or disable rendering for all shadows
	static bool postProcessingEffects; // Enable or disable non-mandatory post processing effects

	// Shadow settings
	// CAN STAY BUT REMOVE DEPENDENCIES (SHADOWS NEED TO MANAGED DIFFERENTLY)
	static ShadowDisk* mainShadowDisk; // Default shadow disk
	static ShadowMap* mainShadowMap;   // Default shadow map

	// Entity links
	static std::vector<Entity*> entityStack; // Stack of all entities that will be processed by the runtime

	// Passes & Pipelines
	static PrePass prePass;
	static ForwardPass forwardPass;
	static SSAOPass ssaoPass;
	static PostProcessingPipeline postProcessingPipeline;

	// Instances
	static QuickGizmo quickGizmo;

	// Perform resize for specific viewport
	static void resizeViewport(float width, float height);

private:
	// Values that only need to be accessed by the runtime itself (eg preprocessors)

	static Camera camera; // Main render camera

	static bool resized; // If window was resized

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