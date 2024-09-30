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
	static glm::vec3 prepare_world_position(glm::vec3 position); // Inverses some of the input position componenets for unity like positioning
	static glm::vec3 prepare_world_rotation(glm::vec3 rotation); // Inverses some of the input rotation componenets for unity like rotation

	static glm::mat4 model_matrix(Entity* entity); // Model matrix
	static glm::mat4 view_matrix(Camera* camera); // View matrix
	static glm::mat4 projection_matrix(Camera* camera, int width, int height); // Projection matrix
	static glm::mat4 mvp(Entity* entity, Camera* camera, int width, int height); // Model-View-Projection matrix
};