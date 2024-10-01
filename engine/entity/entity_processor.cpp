#include "entity_processor.h"
#include "../engine/runtime/runtime.h"

EntityProcessor::EntityProcessor(Entity* entity)
{
	this->entity = entity;
}

void EntityProcessor::render()
{
    if (entity->model == nullptr) return;

    // Calculate mvp
    glm::mat4 model_matrix = Transformation::model_matrix(entity);
    glm::mat4 view_matrix = Transformation::view_matrix(Runtime::renderCamera);
    glm::mat4 projection_matrix = Transformation::projection_matrix(Runtime::renderCamera, Window::width, Window::height);
    glm::mat4 mvp = projection_matrix * view_matrix * model_matrix;

    // Get model
    Model* model = entity->model;

    // Render each mesh of entity
    for (int i = 0; i < entity->model->meshes.size(); i++) {
        Mesh* mesh = model->meshes[i];

        // Bind mesh
        mesh->bind();

        // Get mesh material by index
        IMaterial* material;

        unsigned int materialIndex = mesh->getMaterialIndex();
        if (materialIndex < model->materials.size()) {
            material = model->materials.at(materialIndex);
        }
        else {
            material = Runtime::defaultMaterial;
        }

        // Bind material
        material->bind();

        // Set shader mvp uniform
        material->getShader()->setMatrix4("mvp", mvp);
        material->getShader()->setMatrix4("model", model_matrix);

        // Render mesh
        mesh->render();
    }
}