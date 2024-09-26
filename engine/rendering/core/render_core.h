#pragma once

#include <iostream>

#include "../engine/rendering/shader/shader.h"
#include "../engine/rendering/texture/texture.h"
#include "../engine/rendering/material/material.h"

void render_core_update();

void bindShader(Shader shader);
void bindTexture(Texture texture);

void useMaterial(Material material);
