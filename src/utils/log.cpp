#include "log.h"

#include <iostream>

#include "../src/runtime/runtime.h"

namespace Log {

    void print(TextColor textColor, BackgroundColor bgColor, const std::string& text) {
        // Constructing the ANSI escape code
        std::cout << "\033[" << static_cast<int>(textColor) << "m"
            << "\033[" << static_cast<int>(bgColor) << "m"
            << text
            << "\033[0m" << std::endl;
    }

    std::string getSpacing() {
        return "          ";
    }

    void printError(std::string origin, std::string error, std::string additional_info) {
        print(TextColor::White, BackgroundColor::Red, getSpacing() + origin + " >>> ERROR: " + error + getSpacing());
        if (additional_info != "") {
            print(TextColor::White, BackgroundColor::Black, additional_info);
        }
        Runtime::TERMINATE();
    }

    void printWarning(std::string origin, std::string warning, std::string additional_info) {
        print(TextColor::White, BackgroundColor::Yellow, getSpacing() + origin + " >>> WARNING: " + warning + getSpacing());
        if (additional_info != "") {
            print(TextColor::White, BackgroundColor::Black, additional_info);
        }
    }

    void printProcessStart(std::string origin, std::string process)
    {
        std::cout << std::endl;
        print(TextColor::White, BackgroundColor::Blue, getSpacing() + origin + " >>> " + process + getSpacing());
        std::cout << std::endl;
    }

    void printProcessState(std::string origin, std::string process)
    {
        print(TextColor::White, BackgroundColor::Cyan, getSpacing() + origin + " >>> " + process + getSpacing());
    }

    void printProcessInfo(std::string info)
    {
        print(TextColor::White, BackgroundColor::Black, getSpacing() + info + getSpacing());
    }

    void printProcessDone(std::string origin, std::string process)
    {
        std::cout << std::endl;
        print(TextColor::White, BackgroundColor::Magenta, getSpacing() + origin + " >>> " + process + getSpacing());
        std::cout << std::endl;
    }

    std::string getDepthFunctionName(GLenum func) {
        switch (func) {
        case GL_NEVER: return "GL_NEVER";
        case GL_LESS: return "GL_LESS";
        case GL_EQUAL: return "GL_EQUAL";
        case GL_LEQUAL: return "GL_LEQUAL";
        case GL_GREATER: return "GL_GREATER";
        case GL_NOTEQUAL: return "GL_NOTEQUAL";
        case GL_GEQUAL: return "GL_GEQUAL";
        case GL_ALWAYS: return "GL_ALWAYS";
        default: return "UNKNOWN";
        }
    }

    void printDepthState()
    {
        // Check if GL_DEPTH_TEST is enabled
        GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
        print(TextColor::Yellow, BackgroundColor::Black,
            "GL_DEPTH_TEST: " + std::string(depthTestEnabled ? "Enabled" : "Disabled"));

        // Get the current depth function
        GLint depthFunc;
        glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);
        print(TextColor::Yellow, BackgroundColor::Black,
            "Depth Function: " + getDepthFunctionName(depthFunc));

        // Check if depth mask (writing to depth buffer) is enabled
        GLboolean depthMask;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);
        print(TextColor::Yellow, BackgroundColor::Black,
            "GL_DEPTH_WRITEMASK: " + std::string(depthMask ? "Enabled" : "Disabled"));
    }

}