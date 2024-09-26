#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <stb_image.h>

#include "../engine/rendering/shader/shader.h"
#include "../engine/rendering/shader/shader_builder.h"

#include "../engine/rendering/texture/texture.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/quaternion.hpp>

#include <json/json.hpp>
using json = nlohmann::json;

#include "../engine/utils/log/log.h"

#include "../engine/objects/entity.h"
#include "../engine/objects/camera.h"

#include "../user/src/game_logic.h"

class Runtime {
public:
	static Entity* createEntity();
	static void useCamera(Camera* camera);
};