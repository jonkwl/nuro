#pragma once

//
// CONTAINS ALL UTILITY FUNCTIONS THE DRAWING API USES
//

#include <imgui.h>

namespace DrawUtils {

    inline ImVec2 lerp(const ImVec2& a, const ImVec2& b, float t) {
        return ImVec2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
    }

    inline ImVec4 lerp(const ImVec4& a, const ImVec4& b, float t) {
        return ImVec4(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t);
    }

    inline ImU32 lerp(ImU32 color_a, ImU32 color_b, float t) {
        // Extract individual color channels (RGBA)
        unsigned char a_r = (color_a >> 24) & 0xFF;
        unsigned char a_g = (color_a >> 16) & 0xFF;
        unsigned char a_b = (color_a >> 8) & 0xFF;
        unsigned char a_a = color_a & 0xFF;

        unsigned char b_r = (color_b >> 24) & 0xFF;
        unsigned char b_g = (color_b >> 16) & 0xFF;
        unsigned char b_b = (color_b >> 8) & 0xFF;
        unsigned char b_a = color_b & 0xFF;

        // Interpolate each channel
        unsigned char r = static_cast<unsigned char>(a_r + t * (b_r - a_r));
        unsigned char g = static_cast<unsigned char>(a_g + t * (b_g - a_g));
        unsigned char b = static_cast<unsigned char>(a_b + t * (b_b - a_b));
        unsigned char a_ = static_cast<unsigned char>(a_a + t * (b_a - a_a));

        // Combine channels back into a single ImU32
        return (r << 24) | (g << 16) | (b << 8) | a_;
    }

    inline float lerp(float a, float b, float t) {
        return a + t * (b - a);
    }

}