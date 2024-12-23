#pragma once

#include <cstdint>
#include <vector>
#include <glm.hpp>

#include "../core/rendering/texture/texture.h"
#include "../core/ecs/components.h"

class Shader;
class Model;

class IMGizmo
{
public:
	IMGizmo();

	void setup();    // Load gizmo shaders and models if not loaded already
	void newFrame(); // Clear the render stack

	void renderAll(const glm::mat4& viewProjection); // Renders all gizmos from render stack
	void renderShapes(const glm::mat4& viewProjection); // Renders shapes only from render stack
	void renderIcons(const glm::mat4& viewProjection); // Render icons only from render stack

	// Global render state settings
	glm::vec3 color; // Color of gizmo
	float opacity; // Opacity of gizmo
	bool foreground; // If gizmo should always be in foreground

	// Shapes
	void plane(glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::quat rotation = glm::quat());
	void box(glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::quat rotation = glm::quat());
	void sphere(glm::vec3 position, float radius = 0.5f);
	void planeWire(glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::quat rotation = glm::quat());
	void boxWire(glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::quat rotation = glm::quat());
	void sphereWire(glm::vec3 position, float radius = 0.5f);

	// Icons
	void icon3d(Texture& icon, glm::vec3 position, TransformComponent& cameraTransform, glm::vec3 scale = glm::vec3(1.0f));

public:
	static glm::quat euler(float x, float y, float z);

private:
	struct StaticData {
		bool loaded;

		Shader* fillShader;
		Shader* iconShader;

		Mesh planeMesh;
		Mesh cubeMesh;
		Mesh sphereMesh;
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
		Texture& icon;
		glm::vec3 position;
		glm::vec3 scale;
		TransformComponent& cameraTransform;
		RenderState state;

		IconRenderTarget(Texture& icon, glm::vec3 position, glm::vec3 scale, TransformComponent& cameraTransform, RenderState state) :
			icon(icon),
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
	Mesh& getMesh(Shape shape);
	glm::mat4 getModelMatrix(glm::vec3 position, glm::quat rotation, glm::vec3 scale);
	glm::mat4 getModelMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

	float get3DIconAlpha(float baseAlpha, glm::vec3 iconPosition, glm::vec3 cameraPosition); // Gets the alpha of a 3D icon based on its distance to the camera
};