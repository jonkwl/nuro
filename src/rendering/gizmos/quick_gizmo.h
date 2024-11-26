#pragma once

#include <vector>
#include <glm.hpp>

class UnlitMaterial;
class Model;

struct GizmoColor {
    static constexpr glm::vec3 RED = { 1.0f, 0.0f, 0.0f };
    static constexpr glm::vec3 GREEN = { 0.0f, 1.0f, 0.0f };
    static constexpr glm::vec3 BLUE = { 0.0f, 0.0f, 1.0f };
    static constexpr glm::vec3 YELLOW = { 1.0f, 1.0f, 0.0f };
    static constexpr glm::vec3 CYAN = { 0.0f, 1.0f, 1.0f };
    static constexpr glm::vec3 MAGENTA = { 1.0f, 0.0f, 1.0f };
    static constexpr glm::vec3 WHITE = { 1.0f, 1.0f, 1.0f };
    static constexpr glm::vec3 BLACK = { 0.0f, 0.0f, 0.0f };
    static constexpr glm::vec3 GRAY = { 0.5f, 0.5f, 0.5f };
    static constexpr glm::vec3 DARK_GRAY = { 0.25f, 0.25f, 0.25f };
    static constexpr glm::vec3 LIGHT_GRAY = { 0.75f, 0.75f, 0.75f };
    static constexpr glm::vec3 ORANGE = { 1.0f, 0.5f, 0.0f };
    static constexpr glm::vec3 PURPLE = { 0.5f, 0.0f, 0.5f };
    static constexpr glm::vec3 BROWN = { 0.6f, 0.3f, 0.1f };
    static constexpr glm::vec3 PINK = { 1.0f, 0.75f, 0.8f };
    static constexpr glm::vec3 DARK_RED = { 0.5f, 0.0f, 0.0f };
    static constexpr glm::vec3 DARK_GREEN = { 0.0f, 0.5f, 0.0f };
    static constexpr glm::vec3 DARK_BLUE = { 0.0f, 0.0f, 0.5f };
    static constexpr glm::vec3 DARK_YELLOW = { 0.5f, 0.5f, 0.0f };
    static constexpr glm::vec3 DARK_CYAN = { 0.0f, 0.5f, 0.5f };
    static constexpr glm::vec3 DARK_MAGENTA = { 0.5f, 0.0f, 0.5f };
    static constexpr glm::vec3 DARK_ORANGE = { 0.5f, 0.25f, 0.0f };
    static constexpr glm::vec3 DARK_PURPLE = { 0.25f, 0.0f, 0.25f };
    static constexpr glm::vec3 DARK_BROWN = { 0.3f, 0.15f, 0.05f };
    static constexpr glm::vec3 DARK_PINK = { 0.5f, 0.375f, 0.4f };
};

class QuickGizmo
{
public:
	static void setup(); // Load material and models
    static void newFrame(); // Clear the render stack
    static void render(); // Render all gizmos from render stack

    static glm::vec3 color;

	static void plane(glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f));
	static void box(glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f));
	static void sphere(glm::vec3 position, float radius = 0.5f);

    static void planeWire(glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f));
    static void boxWire(glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f));
    static void sphereWire(glm::vec3 position, float radius = 0.5f);
private:
    enum class Shape {
        PLANE,
        BOX,
        SPHERE
    };

    struct RenderTarget {
        Shape shape;
        glm::vec3 color;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        bool wireframe;
    };

	static UnlitMaterial* material;
	
	static Model* planeModel;
	static Model* cubeModel;
	static Model* sphereModel;

    static std::vector<RenderTarget> renderStack;

    static Model* getModel(Shape shape);
	static glm::mat4 getModelMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
};

