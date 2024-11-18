#include "skybox.h"

Skybox::Skybox(Cubemap* cubemap)
{
    this->cubemap = cubemap;
    this->shader = ShaderPool::get("skybox");
    generate();
}

Skybox::Skybox(Cubemap* cubemap, Shader* custom_shader)
{
	this->cubemap = cubemap;
    this->shader = shader;
    generate();
}

void Skybox::render(glm::mat4 view, glm::mat4 projection)
{
    // DEPTH FUNCTION NEEDS TO BE GL_LEQUAL!
     
    // Set skyvbox shader transformation matrices
    view = glm::mat4(glm::mat3(view));
    shader->bind();
    shader->setMatrix4("view", view);
    shader->setMatrix4("projection", projection);
    shader->setInt("skybox", 0);
    shader->setFloat("emission", emission);

    // Bind skybox vao
    glBindVertexArray(vao);

    // Bind cubemap texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    // Draw skybox
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Skybox::generate()
{
    emission = 1.0f;

    Log::printProcessStart("Skybox", "Generating skybox from cubemap " + cubemap->name + "...");

    if (!create_textures()) return;
    if (!create_buffers()) return;

    Log::printProcessDone("Skybox", "Skybox generated");
}

bool Skybox::create_textures()
{
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    Log::printProcessInfo("Loading textures...");
	for (int i = 0; i < cubemap->faces.size(); i++) {
        CubemapFace face = cubemap->faces[i];

        GLenum format = GL_RGB;
        if (face.channels == 4) {
            format = GL_RGBA;
        }

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, face.width, face.height, 0, format, GL_UNSIGNED_BYTE, face.data.data());
	}

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return true;
}

bool Skybox::create_buffers()
{
    Log::printProcessInfo("Generating sky...");

    // std::vector<float> vertices = get_vertices();
    float vertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

	return true;
}

std::vector<float> Skybox::get_vertices()
{
	return std::vector<float>();
}
