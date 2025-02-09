#pragma once

//
// CONTAINS ALL UTILITY FUNCTIONS
//

#include <cstdint>
#include <imgui.h>
#include <algorithm>

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

    inline void extractRGBA(ImU32 color, uint32_t& r, uint32_t& g, uint32_t& b, uint32_t& a) {
        r = (color >> IM_COL32_R_SHIFT) & 0xFF;
        g = (color >> IM_COL32_G_SHIFT) & 0xFF;
        b = (color >> IM_COL32_B_SHIFT) & 0xFF;
        a = (color >> IM_COL32_A_SHIFT) & 0xFF;
    }

    inline void extractRGB(ImU32 color, uint32_t& r, uint32_t& g, uint32_t& b) {
        r = (color >> IM_COL32_R_SHIFT) & 0xFF;
        g = (color >> IM_COL32_G_SHIFT) & 0xFF;
        b = (color >> IM_COL32_B_SHIFT) & 0xFF;
    }

    inline uint32_t extractRed(ImU32 color) {
        return (color >> IM_COL32_R_SHIFT) & 0xFF;
    }

    inline uint32_t extractGreen(ImU32 color) {
        return (color >> IM_COL32_G_SHIFT) & 0xFF;
    }

    inline uint32_t extractBlue(ImU32 color) {
        return (color >> IM_COL32_B_SHIFT) & 0xFF;
    }

    inline uint32_t extractAlpha(ImU32 color) {
        return (color >> IM_COL32_A_SHIFT) & 0xFF;
    }

    inline ImU32 blend(ImU32 source, ImU32 target) {
        uint32_t sr, sg, sb, sa, tr, tg, tb, ta;

        //
        // Extract RGBA components
        //

        sr = (source >> IM_COL32_R_SHIFT) & 0xFF;
        sg = (source >> IM_COL32_G_SHIFT) & 0xFF;
        sb = (source >> IM_COL32_B_SHIFT) & 0xFF;
        sa = (source >> IM_COL32_A_SHIFT) & 0xFF;

        // Early out if source alpha is fully opaque
        if (sa == 255) {
            return target;
        }

        tr = (target >> IM_COL32_R_SHIFT) & 0xFF;
        tg = (target >> IM_COL32_G_SHIFT) & 0xFF;
        tb = (target >> IM_COL32_B_SHIFT) & 0xFF;
        ta = (target >> IM_COL32_A_SHIFT) & 0xFF;

        // Normalize alpha values to [0, 1] and blend
        float blendedAlpha = ((sa / 255.0f) * (ta / 255.0f)) * 255.0f;

        // Clamp the result to [0, 255] and cast back
        ta = static_cast<uint32_t>(std::clamp(blendedAlpha, 0.0f, 255.0f));

        // Reconstruct the target color with the new alpha
        target = IM_COL32(tr, tg, tb, ta);

        // Return blended target color
        return target;
    }

}