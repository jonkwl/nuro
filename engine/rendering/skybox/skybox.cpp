#include "skybox.h"

Skybox::Skybox(std::string cubemap_source)
{
    this->cubemap_source = cubemap_source;
    this->shader = ShaderBuilder::get("skybox");
    generate();
}

Skybox::Skybox(std::string cubemap_source, Shader* custom_shader)
{
	this->cubemap_source = cubemap_source;
    this->shader = shader;
    generate();
}

void Skybox::draw(glm::mat4 view, glm::mat4 projection)
{
    glDepthFunc(GL_LEQUAL);

    view = glm::mat4(glm::mat3(view));
    shader->bind();
    shader->setMatrix4("view", view);
    shader->setMatrix4("projection", projection);

    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthFunc(GL_LESS);
}

void Skybox::generate()
{
    Log::printProcessStart("Skybox", "Generating skybox...");

    if (!create_textures()) return;
    if (!create_buffers()) return;

    Log::printProcessDone("Skybox", "Skybox generated");
}

bool Skybox::create_textures()
{
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    std::vector<CubemapFace> cubemapFaces;
    Log::printProcessInfo("Extracting cubemap faces...");
    extractCubemap(cubemap_source, cubemapFaces);

    Log::printProcessInfo("Extracting cubemap faces...");
	for (int i = 0; i < cubemapFaces.size(); i++) {
        CubemapFace face = cubemapFaces[i];
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, face.width, face.height, 0, GL_RGB, GL_UNSIGNED_BYTE, face.data.data());

        // stbi_write_png(("./resources/skybox/face_" + std::to_string(i) + ".jpg").c_str(), face.width, face.height, face.channels, face.data.data(), face.width * face.channels);
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
    stbi_set_flip_vertically_on_load(false);

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

void Skybox::extractCubemap(std::string path, std::vector<CubemapFace>& faces)
{
    stbi_set_flip_vertically_on_load(false);

    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data) {
        Log::printError("Skybox", "Failed to load cubemap at " + path);
        return;
    }

    // Calculate dimensions of each face (assuming default 4x3 layout)
    int faceWidth = width / 4;
    int faceHeight = height / 3;

    // Resize faces vector to hold 6 faces
    faces.resize(6);

    // Define offsets for each face
    std::vector<std::pair<int, int>> offsets = {
        {2 * faceWidth, 1 * faceHeight}, // Positive X (Right)
        {0 * faceWidth, 1 * faceHeight}, // Negative X (Left)
        {1 * faceWidth, 0 * faceHeight}, // Positive Y (Top)
        {1 * faceWidth, 2 * faceHeight}, // Negative Y (Bottom)
        {1 * faceWidth, 1 * faceHeight}, // Positive Z (Front)
        {3 * faceWidth, 1 * faceHeight}  // Negative Z (Back)
    };

    // Extract pixel data for each face
    for (size_t i = 0; i < offsets.size(); ++i) {
        int xOffset = offsets[i].first;
        int yOffset = offsets[i].second;

        // Allocate memory for face data
        faces[i].data.resize(faceWidth * faceHeight * channels);
        faces[i].width = faceWidth;
        faces[i].height = faceHeight;
        faces[i].channels = channels;

        // Copy pixel data for each face
        for (int y = 0; y < faceHeight; ++y) {
            for (int x = 0; x < faceWidth; ++x) {
                int srcIndex = ((yOffset + y) * width + (xOffset + x)) * channels;
                int dstIndex = (y * faceWidth + x) * channels;
                memcpy(&faces[i].data[dstIndex], &data[srcIndex], channels);
            }
        }
    }

    // Free loaded image data
    stbi_image_free(data);
}
