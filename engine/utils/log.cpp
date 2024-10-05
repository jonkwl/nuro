#include "log.h"

#include "../engine/runtime/runtime.h"

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

void Log::print(TextColor textColor, BackgroundColor bgColor, const std::string& text) {
    // Constructing the ANSI escape code
    std::cout << "\033[" << static_cast<int>(textColor) << "m"
        << "\033[" << static_cast<int>(bgColor) << "m"
        << text
        << "\033[0m" << std::endl;
}