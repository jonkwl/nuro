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

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/quaternion.hpp>

#include <json/json.hpp>
using json = nlohmann::json;

#include "../engine/rendering/core/render_core.h"
#include "../engine/rendering/shader/shader.h"
#include "../engine/rendering/shader/shader_builder.h"
#include "../engine/rendering/texture/texture.h"
#include "../engine/rendering/mesh/cube_mesh.h"

#include "../engine/objects/entity.h"
#include "../engine/objects/camera.h"

#include "../engine/utils/log.h"
#include "../engine/utils/vector_helper.h"
#include "../engine/utils/inspector_mode.h"

#include "../engine/engineui/engine_ui.h"
#include "../engine/engineui/engine_dialog.h"

#include "../engine/context/context.h"

#include "../engine/input/input.h"

#include "../user/src/game_logic.h"

class Runtime {
public:
	// Entity system
	static std::vector<Entity*> entityLinks; // All entities that will be processed by the runtime
	static Entity* createEntity(); // Creates a new entity and links it with runtime

	// Camera settings
	static Camera* renderCamera; // This camera gets rendered to the screen
	static Camera* activeCamera; // Container camera; Sets render camera value if inspector mode is OFF
	static Camera* inspectorCamera; // Container camera; Sets render camera value if inspector mode is ON
	static void useCamera(Camera* camera);

	// Time & frames
	static float time; // Time passed since application beginning
	static float lastTime; // Time passed since application beginning of last frame
	static float deltaTime; // Frame delta time
	static int fps; // current fps

	// Render settings
	static bool wireframe; // Wireframe option

	// Engine settings
	static bool inspectorMode; // Activates engines inspector mode
	static bool showEngineUI; // Shows engine ui (only available in inspector mode)
	static bool showDiagnostics; // Shows performance diagnostics such as fps etc.
};