#include "runtime_entity.h"
#include "../engine/runtime.h"

RuntimeEntity::RuntimeEntity(Entity* entity)
{
	this->entity = entity;
}

void RuntimeEntity::render()
{
    if (entity->model == nullptr) return;

    // Calculate mvp
    glm::mat4 model_matrix = RenderCore::model_matrix(entity);
    glm::mat4 view_matrix = RenderCore::view_matrix(Runtime::renderCamera);
    glm::mat4 projection_matrix = RenderCore::projection_matrix(Runtime::renderCamera, Context::width, Context::height);
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