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

#include "../engine/entity/entity_processor.h"
#include "../engine/rendering/shader/shader.h"
#include "../engine/rendering/shader/shader_builder.h"
#include "../engine/rendering/material/unlit/unlit_material.h"
#include "../engine/rendering/texture/texture.h"
#include "../engine/rendering/model/model.h"
#include "../engine/rendering/skybox/skybox.h"
#include "../engine/rendering/skybox/cubemap.h"
#include "../engine/rendering/core/forward_pass_frame.h"
#include "../engine/rendering/postprocessing/post_processing.h"
#include "../engine/rendering/prepass/depth_pre_pass.h"
#include "../engine/rendering/shadows/shadow_map.h"
#include "../engine/rendering/primitives/quad.h"
#include "../engine/entity/entity.h"
#include "../engine/camera/camera.h"
#include "../engine/utils/log.h"
#include "../engine/utils/inspector_mode.h"
#include "../engine/utils/profiler.h"
#include "../engine/engineui/engine_ui.h"
#include "../engine/engineui/engine_dialog.h"
#include "../engine/window/window.h"
#include "../engine/input/input.h"
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
	static Shader* depthPrePassShader;
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
	static DepthPrePass* depthPrePass; // Will always be nullptr, not doing any depth pre pass anymore

	// Shadow settings
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