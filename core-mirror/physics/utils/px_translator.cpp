#include "px_translator.h"

namespace PxTranslator {

    // Convert physx::PxVec2 to glm::vec2
    glm::vec2 convert(physx::PxVec2 input) {
        return glm::vec2(input.x, input.y);
    }

    // Convert physx::PxVec3 to glm::vec3
    glm::vec3 convert(physx::PxVec3 input) {
        return glm::vec3(input.x, input.y, input.z);
    }

    // Convert physx::PxVec4 to glm::vec4
    glm::vec4 convert(physx::PxVec4 input) {
        return glm::vec4(input.x, input.y, input.z, input.w);
    }

    // Convert glm::vec2 to physx::PxVec2
    physx::PxVec2 convert(glm::vec2 input) {
        return physx::PxVec2(input.x, input.y);
    }

    // Convert glm::vec3 to physx::PxVec3
    physx::PxVec3 convert(glm::vec3 input) {
        return physx::PxVec3(input.x, input.y, input.z);
    }

    // Convert glm::vec4 to physx::PxVec4
    physx::PxVec4 convert(glm::vec4 input) {
        return physx::PxVec4(input.x, input.y, input.z, input.w);
    }

    // Convert physx::PxQuat to glm::quat
    glm::quat convert(physx::PxQuat input) {
        if (!input.isFinite()) { // invalid input -> identity rotation
            return glm::identity<glm::quat>();
        }
        return glm::quat(input.w, input.x, input.y, input.z);
    }

    // Convert glm::quat to physx::PxQuat
    physx::PxQuat convert(glm::quat input) {
        glm::quat normalized = glm::normalize(input);
        return physx::PxQuat(normalized.x, normalized.y, normalized.z, normalized.w);
    }

}