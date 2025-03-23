#pragma once

#include <glm/glm.hpp>

namespace EditorGizmoColor
{
    static constexpr glm::vec4 CAMERA = { 1.0f, 1.0f, 0.0f, 0.65f };
    static constexpr glm::vec4 COLLIDER = { 0.3f, 1.0f, 0.3f, 0.85f };
    static constexpr glm::vec4 AUDIO_SOURCE_DEFAULT = { 0.5f, 1.0f, 0.8f, 0.05f };
    static constexpr glm::vec4 AUDIO_SOURCE_ACTIVE = { 0.2f, 1.0f, 1.0f, 0.05f };
}