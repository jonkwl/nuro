#pragma once

#include <vector>
#include <glm.hpp>

class Shader;
class Model;

class QuickGizmo
{
public:
    static void setup();    // Load material and models
    static void newFrame(); // Clear the render stack
    static void render();   // Render all gizmos from render stack

    // Global render state settings
    static glm::vec3 color; // Color of gizmo
    static float opacity; // Opacity of gizmo
    static bool foreground; // If gizmo should always be in foreground 

    static void plane(glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f));
    static void box(glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f));
    static void sphere(glm::vec3 position, float radius = 0.5f);

    static void planeWire(glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f));
    static void boxWire(glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f));
    static void sphereWire(glm::vec3 position, float radius = 0.5f);

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

        RenderTarget(Shape shape, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, bool wireframe) :
            shape(shape),
            position(position),
            rotation(rotation),
            scale(scale),
            wireframe(wireframe),
            state(getCurrentState())
        {};
    };

    static Shader *shader;

    static Model *planeModel;
    static Model *cubeModel;
    static Model *sphereModel;

    static std::vector<RenderTarget> renderStack;

    static RenderState getCurrentState();

    static Model *getModel(Shape shape);
    static glm::mat4 getModelMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
};
