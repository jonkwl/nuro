#pragma once

#include <vector>
#include <glm.hpp>
#include <chrono>

#include "../src/rendering/core/pre_pass.h"
#include "../src/rendering/core/forward_pass.h"
#include "../src/rendering/ssao/ssao_pass.h"
#include "../src/rendering/postprocessing/post_processing_pipeline.h"
#include "../src/rendering/skybox/skybox.h"

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

	// All default assets
	static UnlitMaterial* defaultMaterial;
	static Skybox defaultSkybox;

	// Default shaders
	static Shader* prePassShader;
	static Shader* shadowPassShader;
	static Shader* velocityPassShader;

	// Camera settings
	static void useCamera(Camera camera);
	static Camera& getCameraRendering(); // Returns camera actually being rendered
	static Camera& getActiveCamera();	 // Returns camera selected by user (active camera)
	static Camera& getInspectorCamera();

	static float directionalIntensity;
	static glm::vec3 directionalColor;
	static glm::vec3 directionalDirection;
	static glm::vec3 directionalPosition;

	// Skybox settings
	static bool skyboxEnabled;
	static Skybox& selectedSkybox; // Currently selected skybox

	// Time & frames
	static float time;		 // Time passed since application beginning
	static float lastTime;	 // Time passed since application beginning of last frame
	static float deltaTime;	 // Frame delta time
	static float fps;		 // current fps
	static float averageFps; // averaged current fps

	// Render settings
	static glm::vec4 clearColor;	   // buffer clearing color
	static unsigned int msaaSamples;   // MSAA Samples
	static bool vsync;				   // V-Sync option
	static bool wireframe;			   // Wireframe option
	static bool solidMode;			   // Solid mode option (simple render)
	static bool shadows;			   // Enable or disable rendering for all shadows
	static bool postProcessingEffects; // Enable or disable non-mandatory post processing effects

	// Engine settings
	static bool inspectorMode;	 // Activates engines inspector mode
	static bool showEngineUI;	 // Shows engine ui (only available in inspector mode)
	static bool showDiagnostics; // Shows performance diagnostics such as fps etc.

	// Shadow settings
	static ShadowDisk* mainShadowDisk; // Default shadow disk
	static ShadowMap* mainShadowMap;   // Default shadow map

	// Entity links
	static std::vector<Entity*> entityStack; // Stack of all entities that will be processed by the runtime

	// Diagnostics
	static unsigned int currentDrawCalls;
	static unsigned int currentVertices;
	static unsigned int currentPolygons;

	// Passes & Pipelines
	static PrePass prePass;
	static ForwardPass forwardPass;
	static SSAOPass ssaoPass;
	static PostProcessingPipeline postProcessingPipeline;

	// Buffers from passes
	static unsigned int prePassDepthOutput;
	static unsigned int prePassNormalOutput;
	static unsigned int ssaoBuffer;

	// tmp for testing
	static float intensity;
	static float range;
	static float falloff;
	static bool normalMapping;
	static float normalMappingIntensity;
	static unsigned int nCPUEntities;
	static unsigned int nGPUEntities;

private:
	// Values that only need to be accessed by the runtime itself (eg preprocessors)

	static Camera renderCamera;	   // This camera gets rendered to the screen
	static Camera activeCamera;	   // Container camera; Sets render camera value if inspector mode is OFF
	static Camera inspectorCamera; // Container camera; Sets render camera value if inspector mode is ON

	// Averaged fps calculations
	static int averageFpsFrameCount;	// Count of frames in the current second
	static float averageFpsElapsedTime; // Time accumulator for averaging FPS

	static bool resized;

private:
	static void setupGlfw(); // Setup glfw context
	static void setVSync(); // Set v sync option
	static void loadAssets(); // Load all defualt assets
	static void setupScripts(); // Call all scripts needing setup
	static void prepareFrameInternal(); // Prepare internal members for next frame
	static void prepareFrameExternal(); // Prepare external scripts for next frame
	static void renderFrame(); // Render next game frame
	static void renderEditor(); // Render next editor frame
	static void finishFrame(); // Call to finalize current frame
	static void checkResize(); // Perform resize if needed

	static void startupWindow();

	// Context callbacks
	static void glfwErrorCallback(int error, const char* description);
	static void glfwWindowSizeCallback(GLFWwindow* window, int width, int height);
};