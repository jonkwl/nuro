#pragma once

#include <vector>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/quaternion.hpp>

#include "../engine/objects/entity.h"
#include "../engine/objects/camera.h"

class RenderCore
{
public:
	static glm::mat4 model_matrix(Entity* entity);

	static glm::mat4 view_matrix(Camera* camera);

	static glm::mat4 projection_matrix(Camera* camera, int width, int height);

	static glm::mat4 mvp(Entity* entity, Camera* camera, int width, int height);
};