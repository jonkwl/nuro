#pragma once

//
// THIS FILE CONTAINS UTILITY FUNCTIONS NEEDED FOR DRAWING
//

#include <imgui.h>

namespace DrawUtils {

    inline ImVec2 lerp(const ImVec2& a, const ImVec2& b, float t) {
        return ImVec2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
    }

    inline ImVec4 lerp(const ImVec4& a, const ImVec4& b, float t) {
        return ImVec4(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t);
    }

    inline ImU32 lerp(ImU32 a, ImU32 b, float t) {
        return static_cast<ImU32>(a + (b - a) * t);
    }

}
