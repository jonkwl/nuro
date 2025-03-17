#include "console.h"

#include <vector>
#include <iostream>

namespace Console
{

	Printer::Printer() : textColor(Console::TextColor::DEFAULT), backgroundColor(BackgroundColor::NONE)
	{
	}

	Printer::~Printer()
	{
		std::cout 
			<< "\033[0m" 
			<< std::endl;
	}

	Printer& Printer::operator>>(const std::string& text) {
		std::cout
			<< "\033["
			<< static_cast<int>(textColor)
			<< "m" 
			<< "\033["
			<< static_cast<int>(backgroundColor)
			<< "m"
			<< text;
		return *this;
	}

	Printer& Printer::operator>>(TextColor color)
	{
		textColor = color;
		return *this;
	}

	Printer& Printer::operator>>(BackgroundColor color)
	{
		backgroundColor = color;
		return *this;
	}

	Printer& Printer::operator>>(EndLine endLine)
	{
		std::cout << "\033[0m" << std::endl;
		return *this;
	}

	Printer& Printer::operator>>(ResetText resetText)
	{
		textColor = TextColor::DEFAULT;
		return *this;
	}

	Printer& Printer::operator>>(ResetBackground resetBackground)
	{
		backgroundColor = BackgroundColor::NONE;
		return *this;
	}

	namespace out {

		std::vector<LogCallback> logCallbacks;

		void _execCallbacks(const std::string& origin, const std::string& content, LogType logType) {
			for (const auto& callback : logCallbacks) callback(origin, content, logType);
		}

		void registerCallback(LogCallback callback)
		{
			logCallbacks.push_back(callback);
	
		}

		void error(std::string origin, std::string error, std::string additionalInfo)
		{
			print 
				>> " [" >> TextColor::RED 
				>> "error" 
				>> resetText >> "] " 
				>> TextColor::GRAY 
				>> origin >> ": " 
				>> resetText 
				>> error >> endLine;

			if (!additionalInfo.empty()) print >> additionalInfo >> endLine;

			_execCallbacks(origin, error, LogType::ERROR);

			std::exit(-1);
		}

		void warning(std::string origin, std::string warning, std::string additionalInfo)
		{
			print 
				>> " [" 
				>> TextColor::YELLOW 
				>> "warning" 
				>> resetText 
				>> "] " 
				>> TextColor::GRAY 
				>> origin >> ": " 
				>> resetText 
				>> warning 
				>> endLine;

			if (!additionalInfo.empty()) print >> additionalInfo >> endLine;

			_execCallbacks(origin, warning, LogType::WARNING);
		}

		void start(std::string origin, std::string info)
		{
			print 
				>> " [" 
				>> TextColor::BLUE 
				>> "process" 
				>> resetText 
				>> "] " 
				>> TextColor::GRAY 
				>> origin 
				>> ": " 
				>> resetText 
				>> info
				>> "..." 
				>> endLine;

			_execCallbacks(origin, info, LogType::DEFAULT);
		}

		void info(std::string info)
		{
			print
				>> " ["
				>> TextColor::GREEN
				>> "info"
				>> resetText
				>> "] "
				>> TextColor::GRAY
				>> info
				>> endLine;

			_execCallbacks("Info", info, LogType::DEFAULT);
		}

		void info(std::string origin, std::string info)
		{
			print 
				>> " [" 
				>> TextColor::GREEN 
				>> "info" 
				>> resetText 
				>> "] " 
				>> TextColor::GRAY 
				>> origin 
				>> ": " 
				>> resetText 
				>> info
				>> endLine;

			_execCallbacks(origin, info, LogType::DEFAULT);
		}

		void done(std::string origin, std::string info)
		{
			print 
				>> " ["
				>> TextColor::MAGENTA 
				>> "process" 
				>> resetText 
				>> "] " 
				>> TextColor::GRAY 
				>> origin 
				>> ": " 
				>> resetText 
				>> info
				>> "." 
				>> endLine;

			_execCallbacks(origin, info, LogType::DEFAULT);
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
			print >> art >> endLine;
		}

	}

}