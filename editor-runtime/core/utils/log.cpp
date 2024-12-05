#include "log.h"

#include <glad/glad.h>
#include <iostream>

#define EDITOR_CONNECTED 1

#if EDITOR_CONNECTED
#include "../src/ui/windows/console_window.h"
#endif

namespace Log
{

	void _print(TextColor textColor, BackgroundColor bgColor, const std::string& text)
	{
		// Constructing the ANSI escape code
		std::cout << "\033[" << static_cast<int>(textColor) << "m"
			<< "\033[" << static_cast<int>(bgColor) << "m"
			<< text
			<< "\033[0m" << std::endl;
	}

	std::string _getSpacing()
	{
		return "          ";
	}

	void printError(std::string origin, std::string error, std::string additional_info)
	{
		_print(TextColor::White, BackgroundColor::Red, _getSpacing() + origin + " >>> ERROR: " + error + _getSpacing());
		if (additional_info != "")
		{
			_print(TextColor::White, BackgroundColor::Black, additional_info);
		}

#if EDITOR_CONNECTED
		ConsoleWindow::addLog(ConsoleLog{ error, "Core: " + origin, ConsoleLogType::ERROR});
#endif
	}

	void printWarning(std::string origin, std::string warning, std::string additional_info)
	{
		_print(TextColor::Black, BackgroundColor::Yellow, _getSpacing() + origin + " >>> WARNING: " + warning + _getSpacing());
		if (additional_info != "")
		{
			_print(TextColor::White, BackgroundColor::Black, additional_info);
		}

#if EDITOR_CONNECTED
		ConsoleWindow::addLog(ConsoleLog{ warning, "Core: " + origin, ConsoleLogType::WARNING });
#endif
	}

	void printProcessStart(std::string origin, std::string process)
	{
		std::cout << std::endl;
		_print(TextColor::White, BackgroundColor::Blue, _getSpacing() + origin + " >>> " + process + _getSpacing());
		std::cout << std::endl;

#if EDITOR_CONNECTED
		ConsoleWindow::addLog(ConsoleLog{ process, "Core: " + origin, ConsoleLogType::MESSAGE });
#endif
	}

	void printProcessState(std::string origin, std::string process)
	{
		_print(TextColor::White, BackgroundColor::Cyan, _getSpacing() + origin + " >>> " + process + _getSpacing());

#if EDITOR_CONNECTED
		ConsoleWindow::addLog(ConsoleLog{ process, "Core: " + origin, ConsoleLogType::MESSAGE });
#endif
	}

	void printProcessInfo(std::string info)
	{
		_print(TextColor::White, BackgroundColor::Black, _getSpacing() + info + _getSpacing());

#if EDITOR_CONNECTED
		ConsoleWindow::addLog(ConsoleLog{ info, "Core: Log API", ConsoleLogType::MESSAGE});
#endif
	}

	void printProcessDone(std::string origin, std::string process)
	{
		std::cout << std::endl;
		_print(TextColor::White, BackgroundColor::Magenta, _getSpacing() + origin + " >>> " + process + _getSpacing());
		std::cout << std::endl;

#if EDITOR_CONNECTED
		ConsoleWindow::addLog(ConsoleLog{ process, "Core: " + origin, ConsoleLogType::MESSAGE });
#endif
	}

	std::string getDepthFunctionName(GLenum func)
	{
		switch (func)
		{
		case GL_NEVER:
			return "GL_NEVER";
		case GL_LESS:
			return "GL_LESS";
		case GL_EQUAL:
			return "GL_EQUAL";
		case GL_LEQUAL:
			return "GL_LEQUAL";
		case GL_GREATER:
			return "GL_GREATER";
		case GL_NOTEQUAL:
			return "GL_NOTEQUAL";
		case GL_GEQUAL:
			return "GL_GEQUAL";
		case GL_ALWAYS:
			return "GL_ALWAYS";
		default:
			return "UNKNOWN";
		}
	}

	void printDepthState()
	{
		// Check if GL_DEPTH_TEST is enabled
		GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
		_print(TextColor::Yellow, BackgroundColor::Black,
			"GL_DEPTH_TEST: " + std::string(depthTestEnabled ? "Enabled" : "Disabled"));

		// Get the current depth function
		GLint depthFunc;
		glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);
		_print(TextColor::Yellow, BackgroundColor::Black,
			"Depth Function: " + getDepthFunctionName(depthFunc));

		// Check if depth mask (writing to depth buffer) is enabled
		GLboolean depthMask;
		glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);
		_print(TextColor::Yellow, BackgroundColor::Black,
			"GL_DEPTH_WRITEMASK: " + std::string(depthMask ? "Enabled" : "Disabled"));
	}

}