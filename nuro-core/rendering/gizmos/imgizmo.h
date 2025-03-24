#pragma once

#include <vector>
#include <cstdint>
#include <glm/glm.hpp>

#include <ecs/components.h>
#include <memory/resource_manager.h>
#include <rendering/texture/texture.h>

class Shader;
class Model;

class IMGizmo
{
public:
	IMGizmo();

	void create();
	void newFrame();

	//
	// Render funcions
	//

	// Renders all gizmos from render stack
	void renderAll(const glm::mat4& viewProjection);

	// Renders shapes only from render stack
	void renderShapes(const glm::mat4& viewProjection);

	// Render icons only from render stack
	void renderIcons(const glm::mat4& viewProjection);

	//
	// Global render state settings
	//

	// Color of gizmo
	glm::vec3 color;

	// Opacity of gizmo
	float opacity;

	// If gizmo should always be in foreground
	bool foreground;

	// Scale of 3D icons
	float iconScale;

	//
	// Shapes
	//

	void plane(const glm::vec3& position, const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f), const glm::quat& rotation = glm::identity<glm::quat>());
	void box(const glm::vec3& position, const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f), const glm::quat& rotation = glm::identity<glm::quat>());
	void sphere(const glm::vec3& position, float radius = 0.5f, const glm::quat& rotation = glm::identity<glm::quat>());
	
	void planeWire(const glm::vec3& position, const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f), const glm::quat& rotation = glm::identity<glm::quat>());
	void boxWire(const glm::vec3& position, const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f), const glm::quat& rotation = glm::identity<glm::quat>());
	void sphereWire(const glm::vec3& position, float radius = 0.5f, const glm::quat& rotation = glm::identity<glm::quat>());

	//
	// Icons
	//

	void icon3d(uint32_t iconTexture, const glm::vec3& position, TransformComponent& cameraTransform);

private:
	struct StaticData {
		bool loaded = false;

		ResourceRef<Shader> fillShader = nullptr;
		ResourceRef<Shader> iconShader = nullptr;

		const Mesh* planeMesh = nullptr;
		const Mesh* boxMesh = nullptr;
		const Mesh* sphereMesh = nullptr;
	};

	struct RenderState {
		glm::vec3 color;
		float opacity;
		bool foreground;
	};

	enum class Shape
	{
		PLANE,
		BOX,
		SPHERE
	};

	struct ShapeRenderTarget
	{
		Shape shape;
		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;
		bool wireframe;
		RenderState state;

		ShapeRenderTarget(Shape shape, glm::vec3 position, glm::quat rotation, glm::vec3 scale, bool wireframe, RenderState state) :
			shape(shape),
			position(position),
			rotation(rotation),
			scale(scale),
			wireframe(wireframe),
			state(state)
		{};
	};

	struct IconRenderTarget
	{
		uint32_t iconTexture;
		glm::vec3 position;
		float scale;
		TransformComponent& cameraTransform;
		RenderState state;

		IconRenderTarget(uint32_t iconTexture, glm::vec3 position, float scale, TransformComponent& cameraTransform, RenderState state) :
			iconTexture(iconTexture),
			position(position),
			scale(scale),
			cameraTransform(cameraTransform),
			state(state)
		{};
	};

private:
	std::vector<ShapeRenderTarget> shapeRenderStack;
	std::vector<IconRenderTarget> iconRenderStack;

	// Initialized once by any instance being setup, these will be needed for the rest of the application lifetime
	static StaticData staticData;

private:
	RenderState getCurrentState();
	const Mesh* queryMesh(Shape shape);
	glm::mat4 getModelMatrix(glm::vec3 position, glm::quat rotation, glm::vec3 scale);
	glm::mat4 getModelMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

	const Mesh* createPlaneMesh();
	const Mesh* createBoxMesh();
};