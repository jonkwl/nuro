#pragma once

#include <string>

namespace Log
{
    enum class TextColor
    {
        Black = 30,
        Red = 31,
        Green = 32,
        Yellow = 33,
        Blue = 34,
        Magenta = 35,
        Cyan = 36,
        White = 37,
    };

    enum class BackgroundColor
    {
        Black = 40,
        Red = 41,
        Green = 42,
        Yellow = 43,
        Blue = 44,
        Magenta = 45,
        Cyan = 46,
        White = 47,
    };

    void printError(std::string origin, std::string error, std::string additional_info = "");
    void printWarning(std::string origin, std::string warning, std::string additional_info = "");

    void printProcessStart(std::string origin, std::string process);
    void printProcessState(std::string origin, std::string process);
    void printProcessInfo(std::string info);
    void printProcessDone(std::string origin, std::string process);

    void printDepthState();

    unsigned int printUncreatedWarning(std::string origin, std::string action);
};