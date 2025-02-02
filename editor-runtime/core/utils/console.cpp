#include "console.h"

#include <glad/glad.h>
#include <iostream>

#define EDITOR_CONNECTED 1

#if EDITOR_CONNECTED
#include "../src/runtime/runtime.h"
#include "../src/ui/windows/console_window.h"
#endif

namespace Console
{

	void _print(TextColor textColor, BackgroundColor bgColor, const std::string& text)
	{
		// Constructing the ANSI escape code
		std::cout << "\033[" << static_cast<int32_t>(textColor) << "m"
			<< "\033[" << static_cast<int32_t>(bgColor) << "m"
			<< text
			<< "\033[0m" << std::endl;
	}

	std::string _getSpacing()
	{
		return "          ";
	}

	namespace out {

		void error(std::string origin, std::string error, std::string additional_info)
		{
			_print(TextColor::White, BackgroundColor::Red, _getSpacing() + origin + " >>> ERROR: " + error + _getSpacing());
			if (additional_info != "")
			{
				_print(TextColor::White, BackgroundColor::Black, additional_info);
			}

#if EDITOR_CONNECTED
			Runtime::TERMINATE();
#endif
		}

		void warning(std::string origin, std::string warning, std::string additional_info)
		{
			_print(TextColor::Black, BackgroundColor::Yellow, _getSpacing() + origin + " >>> WARNING: " + warning + _getSpacing());
			if (additional_info != "")
			{
				_print(TextColor::White, BackgroundColor::Black, additional_info);
			}

#if EDITOR_CONNECTED
			ConsoleWindow::addLog(ConsoleLog{ "[Internal] " + warning, "Engine Core - " + origin, ConsoleLogType::WARNING });
#endif
		}

		void processStart(std::string origin, std::string process)
		{
			std::cout << std::endl;
			_print(TextColor::White, BackgroundColor::Blue, _getSpacing() + origin + " >>> " + process + _getSpacing());
			std::cout << std::endl;
		}

		void processState(std::string origin, std::string process)
		{
			_print(TextColor::White, BackgroundColor::Cyan, _getSpacing() + origin + " >>> " + process + _getSpacing());
		}

		void processInfo(std::string info)
		{
			_print(TextColor::White, BackgroundColor::Black, _getSpacing() + info + _getSpacing());
		}

		void processDone(std::string origin, std::string process)
		{
			std::cout << std::endl;
			_print(TextColor::White, BackgroundColor::Magenta, _getSpacing() + origin + " >>> " + process + _getSpacing());
			std::cout << std::endl;
		}

		void welcome() {
			const char* art = R"(
    
    
#######################################################################################
#                                                                                     #
#         ________                                                                    #
#        / / / / /                                                                    #
#       / / / / /                                                                     #
#      / / / / /                                                                      #
#     /_/_/_/_/     __        _______ _     ____ ___  __  __ _____ _                  #
#        / / /      \ \      / / ____| |   / ___/ _ \|  \/  | ____| |                 #
#       / / /___     \ \ /\ / /|  _| | |  | |  | | | | |\/| |  _| | |                 #
#      / / / / /      \ V  V / | |___| |__| |__| |_| | |  | | |___|_|                 #
#     /_/_/ / /        \_/\_/  |_____|_____\____\___/|_|  |_|_____(_)                 #
#        / / /                                                                        #
#       /_/_/_____                                                                    #
#        / / / / /                                                                    #
#       / / / / /                                                                     #
#      / / / / /                                                                      #
#     /_/_/_/_/                                                                       #
#                                                                                     #
#######################################################################################

)";
			std::cout << art << std::endl;
		}

		int main() {
			welcome();
			return 0;
		}


	}

}