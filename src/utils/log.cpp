#include "log.h"

#include "../src/runtime/runtime.h"

void Log::printError(std::string origin, std::string error, std::string additional_info) {
    print(TextColor::White, BackgroundColor::Red, getBuffer() + origin + " >>> ERROR: " + error + getBuffer());
    if (additional_info != "") {
        print(TextColor::White, BackgroundColor::Black, additional_info);
    }
    Runtime::TERMINATE();
}

void Log::printWarning(std::string origin, std::string warning, std::string additional_info) {
    print(TextColor::White, BackgroundColor::Yellow, getBuffer() + origin + " >>> WARNING: " + warning + getBuffer());
    if (additional_info != "") {
        print(TextColor::White, BackgroundColor::Black, additional_info);
    }
}

void Log::printProcessStart(std::string origin, std::string process)
{
    std::cout << std::endl;
    print(TextColor::White, BackgroundColor::Blue , getBuffer() + origin + " >>> " + process + getBuffer());
    std::cout << std::endl;
}

void Log::printProcessState(std::string origin, std::string process)
{
    print(TextColor::White, BackgroundColor::Cyan, getBuffer() + origin + " >>> " + process + getBuffer());
}

void Log::printProcessInfo(std::string info)
{
    print(TextColor::White, BackgroundColor::Black, getBuffer() + info + getBuffer());
}

void Log::printProcessDone(std::string origin, std::string process)
{
    std::cout << std::endl;
    print(TextColor::White, BackgroundColor::Magenta, getBuffer() + origin + " >>> " + process + getBuffer());
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

void Log::printDepthState()
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

void Log::print(TextColor textColor, BackgroundColor bgColor, const std::string& text) {
    // Constructing the ANSI escape code
    std::cout << "\033[" << static_cast<int>(textColor) << "m"
        << "\033[" << static_cast<int>(bgColor) << "m"
        << text
        << "\033[0m" << std::endl;
}