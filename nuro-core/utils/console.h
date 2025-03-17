#pragma once

#include <string>
#include <functional>

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

	enum class BackgroundColor
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
	class ResetBackground {};

	//
	// PRINTER
	//

	class Printer {
	private:
		TextColor textColor;
		BackgroundColor backgroundColor;

	public:
		Printer();
		~Printer();

		Printer& operator>>(const std::string& text);
		Printer& operator>>(TextColor color);
		Printer& operator>>(BackgroundColor color);

		Printer& operator>>(EndLine endLine);
		Printer& operator>>(ResetText resetText);
		Printer& operator>>(ResetBackground resetBackground);

	};

	//
	// INSTANCES
	//

	inline EndLine endLine;
	inline ResetText resetText;
	inline ResetBackground resetBackground;
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

		// Callback for a log event with origin, content and log type
		using LogCallback = std::function<void(const std::string&, const std::string&, LogType)>;

		// Registers a log callback for engine outputs
		void registerCallback(LogCallback callback);

		// Prints an error and terminates the program if possible
		void error(std::string origin, std::string error, std::string additionalInfo = "");

		// Prints a warning
		void warning(std::string origin, std::string warning, std::string additionalInfo = "");

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