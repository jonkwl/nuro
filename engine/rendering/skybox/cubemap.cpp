#include "cubemap.h"

Cubemap* Cubemap::GetBySingle(std::string path)
{
    Cubemap* cubemap = new Cubemap();
    cubemap->name = IOHandler::GetFilenameRaw(path);

    Log::printProcessStart("Cubemap", "Generating cubemap " + cubemap->name + "...");

    cubemap->extractFaces(path);

    Log::printProcessDone("Cubemap", "Cubemap generated");

    return cubemap;
}

Cubemap* Cubemap::GetByFaces(std::string right, std::string left, std::string top, std::string bottom, std::string front, std::string back)
{
    Cubemap* cubemap = new Cubemap();
    cubemap->name = IOHandler::GetLastFolder(right);

    Log::printProcessStart("Cubemap", "Generating cubemap " + cubemap->name + "...");

    cubemap->loadFace(right);
    cubemap->loadFace(left);
    cubemap->loadFace(top);
    cubemap->loadFace(bottom);
    cubemap->loadFace(front);
    cubemap->loadFace(back);

    Log::printProcessDone("Cubemap", "Cubemap generated");

    return cubemap;
}

void Cubemap::extractFaces(std::string path)
{
    stbi_set_flip_vertically_on_load(false);

    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data) {
        Log::printError("Cubemap", "Failed to load cubemap at " + path);
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

void Cubemap::loadFace(std::string path)
{
    stbi_set_flip_vertically_on_load(false);

    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data) {
        Log::printError("Cubemap", "Failed to load cubemap at " + path);
        return;
    }

    std::vector<unsigned char> faceData(data, data + (width * height * channels));

    CubemapFace face;
    face.data = faceData;
    face.width = width;
    face.height = height;
    face.channels = channels;
    faces.push_back(face);
}