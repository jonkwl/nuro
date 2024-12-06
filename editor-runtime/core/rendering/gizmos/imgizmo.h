#pragma once

#include <vector>
#include <glm.hpp>

class Shader;
class Model;

class IMGizmo
{
public:
	IMGizmo();

	void setup();    // Load gizmo shaders and models if not loaded already
	void newFrame(); // Clear the render stack
	void render();   // Render all gizmos from render stack

	// Global render state settings
	glm::vec3 color; // Color of gizmo
	float opacity; // Opacity of gizmo
	bool foreground; // If gizmo should always be in foreground

	void plane(glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f));
	void box(glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f));
	void sphere(glm::vec3 position, float radius = 0.5f);

	void planeWire(glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f));
	void boxWire(glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f));
	void sphereWire(glm::vec3 position, float radius = 0.5f);

private:
	enum class Shape
	{
		PLANE,
		BOX,
		SPHERE
	};
	struct RenderState {
		glm::vec3 color;
		float opacity;
		bool foreground;
	};
	struct RenderTarget
	{
		Shape shape;
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
		bool wireframe;
		RenderState state;

		RenderTarget(Shape shape, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, bool wireframe, RenderState state) :
			shape(shape),
			position(position),
			rotation(rotation),
			scale(scale),
			wireframe(wireframe),
			state(state)
		{};
	};

private:
	RenderState getCurrentState();
	Model* getModel(Shape shape);
	glm::mat4 getModelMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

private:
	std::vector<RenderTarget> renderStack;

	// Initialized once by any instance, these will be needed for the rest of the application lifetime
	static Shader* shader;
	static Model* planeModel;
	static Model* cubeModel;
	static Model* sphereModel;
};