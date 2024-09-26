#pragma once

#include <string>
#include <iostream>

class Log
{
public:
    enum class TextColor {
        Black = 30,
        Red = 31,
        Green = 32,
        Yellow = 33,
        Blue = 34,
        Magenta = 35,
        Cyan = 36,
        White = 37,
    };

    enum class BackgroundColor {
        Black = 40,
        Red = 41,
        Green = 42,
        Yellow = 43,
        Blue = 44,
        Magenta = 45,
        Cyan = 46,
        White = 47,
    };

    static void printError(std::string origin, std::string error);

    static void printProcessStart(std::string origin, std::string process);
    static void printProcessState(std::string origin, std::string process);
    static void printProcessInfo(std::string info);
    static void printProcessDone(std::string origin, std::string process);

private:
    static std::string getBuffer() {
        return "          ";
    }
    static void print(TextColor textColor, BackgroundColor bgColor, const std::string& text);
};

