#pragma once

#include <vector>
#include <glm.hpp>

#include "../src/rendering/material/unlit/unlit_material.h"
#include "../src/rendering/model/model.h"

struct GizmoColor {
    static const glm::vec3 RED;
    static const glm::vec3 GREEN;
    static const glm::vec3 BLUE;
    static const glm::vec3 YELLOW;
    static const glm::vec3 CYAN;
    static const glm::vec3 MAGENTA;
    static const glm::vec3 WHITE;
    static const glm::vec3 BLACK;
    static const glm::vec3 GRAY;
    static const glm::vec3 DARK_GRAY;
    static const glm::vec3 LIGHT_GRAY;
    static const glm::vec3 ORANGE;
    static const glm::vec3 PURPLE;
    static const glm::vec3 BROWN;
    static const glm::vec3 PINK;
    static const glm::vec3 DARK_RED;
    static const glm::vec3 DARK_GREEN;
    static const glm::vec3 DARK_BLUE;
    static const glm::vec3 DARK_YELLOW;
    static const glm::vec3 DARK_CYAN;
    static const glm::vec3 DARK_MAGENTA;
    static const glm::vec3 DARK_ORANGE;
    static const glm::vec3 DARK_PURPLE;
    static const glm::vec3 DARK_BROWN;
    static const glm::vec3 DARK_PINK;
};

enum GizmoShape {
    PLANE,
    BOX,
    SPHERE
};

struct GizmoRenderTarget {
    GizmoShape shape;
    glm::vec3 color;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    bool wireframe;
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
	static UnlitMaterial* material;
	
	static Model* planeModel;
	static Model* cubeModel;
	static Model* sphereModel;

    static std::vector<GizmoRenderTarget> renderStack;

    static Model* getModel(GizmoShape shape);
	static glm::mat4 getModelMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
};

