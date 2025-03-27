#pragma once

#include <string>
#include <functional>

#include <utils/event.h>

namespace Console
{

	//
	// TEXT COLORS
	//

	enum class TextColor
	{
		DEFAULT = 39,
		BLACK = 30,
		RED = 31,
		GREEN = 32,
		YELLOW = 33,
		BLUE = 34,
		MAGENTA = 35,
		CYAN = 36,
		WHITE = 37,
		GRAY = 90
	};

	//
	// BACKGROUND COLORS
	//

	enum class BgColor
	{
		NONE = 49,
		BLACK = 40,
		RED = 41,
		GREEN = 42,
		YELLOW = 43,
		BLUE = 44,
		MAGENTA = 45,
		CYAN = 46,
		WHITE = 47	
	};

	//
	// OUTPUT TAGS
	//

	class EndLine {};
	class ResetText {};
	class ResetBg {};

	//
	// PRINTER
	//

	class Printer {
	private:
		TextColor textColor;
		BgColor bgColor;

	public:
		Printer();
		~Printer();

		Printer& operator>>(const std::string& text);
		Printer& operator>>(TextColor color);
		Printer& operator>>(BgColor color);

		Printer& operator>>(EndLine endl);
		Printer& operator>>(ResetText resetText);
		Printer& operator>>(ResetBg resetBg);

	};

	//
	// INSTANCES
	//

	inline EndLine endl;
	inline ResetText resetText;
	inline ResetBg resetBg;
	inline Printer print;

	//
	// PREFABRICATED OUTPUT
	//

	namespace out {

		enum class LogType {
			DEFAULT,
			WARNING,
			ERROR
		};

		// Returns the log event (parameters: origin, content and log type)
		Event<const std::string&, const std::string&, LogType>& logEvent();

		// Prints an error
		void error(std::string origin, std::string error, std::string additionalInfo = "");

		// Prints a warning
		void warning(std::string origin, std::string warning, std::string additionalInfo = "");

		// Prints that some specified process has started
		void start(std::string origin, std::string info);

		// Prints an information
		void info(std::string info);

		// Prints an information
		void info(std::string origin, std::string info);

		// Prints that some specified process is done
		void done(std::string origin, std::string info);

		// Prints a highlighted debug message
		void debug(std::string origin, std::string info);

		// Prints the ASCII welcome message
		void welcome();

	}
};