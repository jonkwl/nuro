#include "transform.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <ecs/ecs.h>
#include <rendering/transformation/transformation.h>

namespace Transform {

	bool hasParent(TransformComponent& transform)
	{
		return transform.parent != entt::null;
	}

	TransformComponent& fetchParent(TransformComponent& transform)
	{
		return ECS::gRegistry.get<TransformComponent>(transform.parent);
	}

	void evaluate(TransformComponent& transform)
	{
		// Transform has parent
		if (hasParent(transform)) {

			TransformComponent& parent = fetchParent(transform);

			// Set transform to be modified if parent is modified
			if (parent.modified) transform.modified = true;

			// Only evaluate transform if it has been modified
			if (!transform.modified) return;

			// Evaluate transforms model matrix relative to parent
			transform.model = parent.model * Transformation::model(transform.position, transform.rotation, transform.scale);

		}
		// Transform doesn't have parent
		else {

			// Only evaluate transform if it has been modified
			if (!transform.modified) return;

			// Evaluate transforms model matrix
			transform.model = Transformation::model(transform.position, transform.rotation, transform.scale);

		}

		// Compute transforms normal matrix
		transform.normal = Transformation::normal(transform.model);
	}

	void evaluateRecursive(TransformComponent& transform)
	{
		// Evaluate transform
		evaluate(transform);

		// Evaluate children recursively
		for (Entity child : transform.children) {
			evaluateRecursive(ECS::gRegistry.get<TransformComponent>(child));
		}
	}

	void evaluateReversed(TransformComponent& transform)
	{
		if (hasParent(transform)) {
			TransformComponent& parent = fetchParent(transform);
			evaluateReversed(parent);
		}

		evaluate(transform);
		transform.modified = false;
	}

	void evaluateMvp(TransformComponent& transform, const glm::mat4& viewProjection)
	{
		transform.mvp = viewProjection * transform.model;
	}

	void setPosition(TransformComponent& transform, const glm::vec3& position, Space space)
	{
		// Local space
		if (space == Space::LOCAL || !hasParent(transform)) {
			transform.position = position;
		}
		// World space with parent
		else {
			TransformComponent& parent = fetchParent(transform);
			if (parent.modified) evaluateReversed(parent);

			glm::vec4 localBackendPos = glm::vec4(Transformation::swap(position), 1.0f);
			glm::vec3 worldBackendPos = glm::vec3(glm::inverse(parent.model) * localBackendPos);

			transform.position = Transformation::swap(worldBackendPos);
		}

		transform.modified = true;
	}

	void setRotation(TransformComponent& transform, const glm::quat& rotation, Space space)
	{
		// Local space
		if (space == Space::LOCAL || !hasParent(transform)) {
			transform.rotation = rotation;
			transform.eulerAngles = toEuler(rotation);
		}
		// World space with parent
		else {
			TransformComponent& parent = fetchParent(transform);
			if (parent.modified) evaluateReversed(parent);

			transform.rotation = glm::inverse(parent.rotation) * rotation;
			transform.eulerAngles = toEuler(transform.rotation);
		}

		transform.modified = true;
	}

	void setEulerAngles(TransformComponent& transform, const glm::vec3& eulerAngles, Space space)
	{
		glm::quat rotation = glm::quat(glm::radians(eulerAngles));

		// Local space
		if (space == Space::LOCAL || !hasParent(transform)) {
			transform.rotation = rotation;
			transform.eulerAngles = eulerAngles;
		}
		// World space with parent
		else {
			TransformComponent& parent = fetchParent(transform);
			if (parent.modified) evaluateReversed(parent);

			transform.rotation = glm::inverse(parent.rotation) * rotation;
			transform.eulerAngles = toEuler(transform.rotation);
		}

		transform.modified = true;
	}

	void setScale(TransformComponent& transform, const glm::vec3& scale, Space space)
	{
		// Local space
		if (space == Space::LOCAL || !hasParent(transform)) {
			transform.scale = scale;
		}
		// World space with parent
		else {
			TransformComponent& parent = fetchParent(transform);
			if (parent.modified) evaluateReversed(parent);

			glm::mat4 parentModel = parent.model;
			glm::vec3 parentWorldScale = glm::vec3(
				glm::length(glm::vec3(parentModel[0])),
				glm::length(glm::vec3(parentModel[1])),
				glm::length(glm::vec3(parentModel[2]))
			);

			transform.scale = scale / parentWorldScale;
		}

		transform.modified = true;
	}

	glm::vec3 getPosition(TransformComponent& transform, Space space)
	{
		// Local space
		if (space == Space::LOCAL || !hasParent(transform)) {
			return transform.position;
		}
		// World space with parent
		else {
			if (transform.modified) evaluateReversed(transform);

			return Transformation::swap(glm::vec3(transform.model[3]));
		}
	}

	glm::quat getRotation(TransformComponent& transform, Space space)
	{
		// Local space
		if (space == Space::LOCAL || !hasParent(transform)) {
			return transform.rotation;
		}
		// World space with parent
		else {
			if (transform.modified) evaluateReversed(transform);

			glm::mat3 rotationMatrix(transform.model);

			glm::vec3 col0 = glm::normalize(glm::vec3(rotationMatrix[0]));
			glm::vec3 col1 = glm::normalize(glm::vec3(rotationMatrix[1]));
			glm::vec3 col2 = glm::normalize(glm::vec3(rotationMatrix[2]));

			rotationMatrix[0] = col0;
			rotationMatrix[1] = col1;
			rotationMatrix[2] = col2;

			return Transformation::swap(glm::quat_cast(rotationMatrix));
		}
	}

	glm::vec3 getEulerAngles(TransformComponent& transform, Space space)
	{
		// Local space
		if (space == Space::LOCAL || !hasParent(transform)) {
			return transform.eulerAngles;
		}
		// World space with parent
		else {
			return glm::vec3(0.0f);
		}
	}

	glm::vec3 getScale(TransformComponent& transform, Space space)
	{
		// Local space
		if (space == Space::LOCAL || !hasParent(transform)) {
			return transform.scale;
		}
		// World space with parent
		else {
			if (transform.modified) evaluateReversed(transform);

			return glm::vec3(
				glm::length(glm::vec3(transform.model[0])),
				glm::length(glm::vec3(transform.model[1])),
				glm::length(glm::vec3(transform.model[2]))
			);
		}
	}

	void translate(TransformComponent& transform, const glm::vec3& position, Space space)
	{
		// Local space
		if (space == Space::LOCAL || !hasParent(transform)) {
			transform.position += position;
		}
		// World space with parent
		else {

		}

		transform.modified = true;
	}

	void rotate(TransformComponent& transform, const glm::quat& rotation, Space space)
	{
		// Local space
		if (space == Space::LOCAL || !hasParent(transform)) {
			transform.rotation = rotation * transform.rotation;
		}
		// World space with parent
		else {

		}

		transform.modified = true;
	}

	void scale(TransformComponent& transform, const glm::vec3& scale, Space space)
	{
		// Local space
		if (space == Space::LOCAL || !hasParent(transform)) {
			transform.scale *= scale;
		}
		// World space with parent
		else {

		}

		transform.modified = true;
	}

	glm::vec3 _direction(glm::vec3 base, TransformComponent& transform, Space space) {
		if (space == Space::LOCAL) {
			return glm::rotate(transform.rotation, base);
		}
		else {
			return glm::rotate(getRotation(transform, Space::WORLD), base);
		}
	}

	glm::vec3 forward(TransformComponent& transform, Space space)
	{
		return _direction(glm::vec3(0.0f, 0.0f, 1.0f), transform, space);
	}

	glm::vec3 backward(TransformComponent& transform, Space space)
	{
		return _direction(glm::vec3(0.0f, 0.0f, -1.0f), transform, space);;
	}

	glm::vec3 right(TransformComponent& transform, Space space)
	{
		return _direction(glm::vec3(1.0f, 0.0f, 0.0f), transform, space);;
	}

	glm::vec3 left(TransformComponent& transform, Space space)
	{
		return _direction(glm::vec3(-1.0f, 0.0f, 0.0f), transform, space);;
	}

	glm::vec3 up(TransformComponent& transform, Space space)
	{
		return _direction(glm::vec3(0.0f, 1.0f, 0.0f), transform, space);;
	}

	glm::vec3 down(TransformComponent& transform, Space space)
	{
		return _direction(glm::vec3(0.0f, -1.0f, 0.0f), transform, space);;
	}

	glm::quat lookAt(const glm::vec3& position, const glm::vec3& target)
	{
		glm::vec3 direction = glm::normalize(target - position);
		glm::vec3 forwardLocal = position;
		return glm::rotation(forwardLocal, direction);
	}

	glm::quat toQuat(glm::vec3 eulerAngles)
	{
		return glm::quat(glm::radians(eulerAngles));
	}

	glm::vec3 toEuler(const glm::quat& quaternion)
	{
		return glm::degrees(glm::eulerAngles(quaternion));
	}
}