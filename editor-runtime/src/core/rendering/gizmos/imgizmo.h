#pragma once

#include <cstdint>
#include <vector>
#include <glm.hpp>

#include "../src/core/rendering/texture/texture.h"
#include "../src/core/ecs/components.h"

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

	//
	// Shapes
	//

	void plane(glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::quat rotation = glm::identity<glm::quat>());
	void box(glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::quat rotation = glm::identity<glm::quat>());
	void sphere(glm::vec3 position, float radius = 0.5f, glm::quat rotation = glm::identity<glm::quat>());
	void planeWire(glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::quat rotation = glm::identity<glm::quat>());
	void boxWire(glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::quat rotation = glm::identity<glm::quat>());
	void sphereWire(glm::vec3 position, float radius = 0.5f, glm::quat rotation = glm::identity<glm::quat>());

	//
	// Icons
	//

	void icon3d(uint32_t iconTexture, glm::vec3 position, TransformComponent& cameraTransform, glm::vec3 scale = glm::vec3(1.0f));

private:
	struct StaticData {
		bool loaded = false;

		Shader* fillShader = nullptr;
		Shader* iconShader = nullptr;

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
		glm::vec3 scale;
		TransformComponent& cameraTransform;
		RenderState state;

		IconRenderTarget(uint32_t iconTexture, glm::vec3 position, glm::vec3 scale, TransformComponent& cameraTransform, RenderState state) :
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

	// Returns the alpha of a 3D icon depending on its distance to the camera
	float get3DIconAlpha(float baseAlpha, glm::vec3 iconPosition, glm::vec3 cameraPosition);

	const Mesh* createWireframeBox();
};