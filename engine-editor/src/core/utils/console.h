#pragma once

#include <cstdint>
#include <string>

namespace Console
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

	namespace out {

		// Prints an error and terminates the program if possible
		void error(std::string origin, std::string error, std::string additional_info = "");

		// Prints a warning
		void warning(std::string origin, std::string warning, std::string additional_info = "");

		// Prints that some specified process has started
		void processStart(std::string origin, std::string process);

		// Prints the state of some process
		void processState(std::string origin, std::string process);

		// Prints an information from some process without providing information about the process
		void processInfo(std::string info);

		// Prints that some specified process is done
		void processDone(std::string origin, std::string process);

		// Prints the ASCII welcome message
		void welcome();

	}
};