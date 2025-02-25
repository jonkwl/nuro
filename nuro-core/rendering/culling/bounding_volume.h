#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <rendering/gizmos/gizmos.h>

class Model;

class BoundingVolume
{
public:
	virtual void update(Model* model, glm::vec3 position, glm::quat rotation, glm::vec3 scale) {};
	virtual bool intersectsFrustum() { return false; };
	virtual float getDistance(glm::vec3 point) { return 0.0f; }
	virtual void draw(IMGizmo& imGizmoInstance, glm::vec4 color) {};
};

class BoundingSphere : public BoundingVolume
{
public:
	BoundingSphere();

	void update(Model* model, glm::vec3 position, glm::quat rotation, glm::vec3 scale);
	bool intersectsFrustum();
	float getDistance(glm::vec3 point);
	void draw(IMGizmo& imGizmoInstance, glm::vec4 color);

	glm::vec3 center;
	float radius;
};

class BoundingAABB : public BoundingVolume
{
public:
	BoundingAABB();

	void update(Model* model, glm::vec3 position, glm::quat rotation, glm::vec3 scale);
	bool intersectsFrustum();
	float getDistance(glm::vec3 point);
	void draw(IMGizmo& imGizmoInstance, glm::vec4 color);

	glm::vec3 min;
	glm::vec3 max;
};
