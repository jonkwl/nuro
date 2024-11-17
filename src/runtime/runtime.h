#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
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

#include <json/json.hpp>
using json = nlohmann::json;

#include "../src/entity/entity_processor.h"
#include "../src/rendering/shader/shader.h"
#include "../src/rendering/shader/shader_builder.h"
#include "../src/rendering/material/unlit/unlit_material.h"
#include "../src/rendering/texture/texture.h"
#include "../src/rendering/model/model.h"
#include "../src/rendering/skybox/skybox.h"
#include "../src/rendering/skybox/cubemap.h"
#include "../src/rendering/core/forward_pass_frame.h"
#include "../src/rendering/postprocessing/post_processing.h"
#include "../src/rendering/core/pre_pass.h"
#include "../src/rendering/shadows/shadow_map.h"
#include "../src/rendering/shadows/shadow_disk.h"
#include "../src/rendering/primitives/quad.h"
#include "../src/entity/entity.h"
#include "../src/camera/camera.h"
#include "../src/utils/log.h"
#include "../src/utils/inspector_mode.h"
#include "../src/utils/profiler.h"
#include "../src/engineui/engine_ui.h"
#include "../src/window/window.h"
#include "../src/input/input.h"
#include "../user/src/game_logic.h"

class Runtime {
public:
	static int START_LOOP(); // Main loop
	static void TERMINATE(); // Terminate and exit

	// Entity system
	static void linkEntity(Entity* entity); // Links a given entity with the runtime (creating a runtime entity)

	// All default assets
	static UnlitMaterial* defaultMaterial;
	static Cubemap* defaultSky;
	static Skybox* defaultSkybox;

	// Default shaders
	static Shader* prePassShader;
	static Shader* shadowPassShader;

	// Camera settings
	static void useCamera(Camera* camera);
	static Camera* getCameraRendering(); // Returns camera actually being rendered
	static Camera* getActiveCamera(); // Returns camera selected by user (active camera)
	static Camera* getInspectorCamera();

	static float directionalIntensity;
	static glm::vec3 directionalColor;
	static glm::vec3 directionalDirection;
	static glm::vec3 directionalPosition;

	// Skybox settings
	static void setSkybox(Skybox* skybox); // Set active skybox
	static Skybox* getActiveSkybox(); // Get active skybox

	// Time & frames
	static float time; // Time passed since application beginning
	static float lastTime; // Time passed since application beginning of last frame
	static float deltaTime; // Frame delta time
	static int fps; // current fps
	static float averageFps; // averaged current fps

	// Render settings
	static glm::vec4 clearColor; // buffer clearing color
	static unsigned int msaaSamples; // MSAA Samples
	static bool vsync; // V-Sync option
	static bool wireframe; // Wireframe option
	static bool solidMode; // Solid mode option (simple render)
	static bool shadows; // Enable or disable rendering for all shadows
	static bool postProcessingEffects; // Enable or disable non-mandatory post processing effects

	// Engine settings
	static bool inspectorMode; // Activates engines inspector mode
	static bool showEngineUI; // Shows engine ui (only available in inspector mode)
	static bool showDiagnostics; // Shows performance diagnostics such as fps etc.

	// Pre Pass
	static PrePass* prePass; // (Unused) Render pipeline pre pass

	// Shadow settings
	static ShadowDisk* mainShadowDisk; // Default shadow disk
	static ShadowMap* mainShadowMap; // Default shadow map

	// Return data
	static std::vector<EntityProcessor*> getEntityLinks();

	// Diagnostics
	static unsigned int currentDrawCalls;
	static unsigned int currentVertices;
	static unsigned int currentPolygons;

	// tmp for material testing
	static float intensity;
	static float range;
	static float falloff;

	static bool normalMapping;
	static float normalMappingIntensity;
private:
	static std::vector<EntityProcessor*> entityLinks; // All entities that will be processed by the runtime

	static Camera* renderCamera; // This camera gets rendered to the screen
	static Camera* activeCamera; // Container camera; Sets render camera value if inspector mode is OFF
	static Camera* inspectorCamera; // Container camera; Sets render camera value if inspector mode is ON

	static Skybox* activeSkybox; // Currently selected skybox

	// Averaged fps calculations
	static int averageFpsFrameCount; // Count of frames in the current second
	static float averageFpsElapsedTime; // Time accumulator for averaging FPS
};