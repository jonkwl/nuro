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
    glm::mat4 mvp = RenderCore::mvp(entity, Runtime::renderCamera, Context::width, Context::height);

    for (int i = 0; i < entity->model->meshes.size(); i++) {
        entity->model->meshes[i]->bind();

        // Set bind material (handles shader and texture binding and preset shader uniforms)
        entity->model->material->bind();

        // Set shader mvp uniform
        entity->model->material->getShader()->setMatrix4("mvp", mvp);

        entity->model->meshes[i]->render();
    }
}