#include "console.h"

#include <vector>
#include <iostream>

namespace Console
{

	Printer::Printer() : textColor(Console::TextColor::DEFAULT), bgColor(BgColor::NONE)
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
			<< static_cast<int>(bgColor)
			<< "m"
			<< text;
		return *this;
	}

	Printer& Printer::operator>>(TextColor color)
	{
		textColor = color;
		return *this;
	}

	Printer& Printer::operator>>(BgColor color)
	{
		bgColor = color;
		return *this;
	}

	Printer& Printer::operator>>(EndLine endl)
	{
		std::cout << "\033[0m" << std::endl;
		return *this;
	}

	Printer& Printer::operator>>(ResetText resetText)
	{
		textColor = TextColor::DEFAULT;
		return *this;
	}

	Printer& Printer::operator>>(ResetBg resetBg)
	{
		bgColor = BgColor::NONE;
		return *this;
	}

	namespace out {

		Event<const std::string&, const std::string&, LogType> gLogEvent;

		Event<const std::string&, const std::string&, LogType>& logEvent()
		{
			return gLogEvent;
		}

		void error(std::string origin, std::string error, std::string additionalInfo)
		{
			print 
				>> " nuro >>> [" 
				>> TextColor::RED 
				>> "error" 
				>> resetText >> "] " 
				>> TextColor::GRAY 
				>> origin >> ": " 
				>> resetText 
				>> error 
				>> endl;

			if (!additionalInfo.empty()) print >> " " >> additionalInfo >> endl;

			gLogEvent(origin, error, LogType::ERROR);

			std::exit(-1);
		}

		void warning(std::string origin, std::string warning, std::string additionalInfo)
		{
			print 
				>> " nuro >>> ["
				>> TextColor::YELLOW 
				>> "warning" 
				>> resetText 
				>> "] " 
				>> TextColor::GRAY 
				>> origin >> ": " 
				>> resetText 
				>> warning 
				>> endl;

			if (!additionalInfo.empty()) print >> " " >> additionalInfo >> endl;

			gLogEvent(origin, warning, LogType::WARNING);
		}

		void start(std::string origin, std::string info)
		{
			print 
				>> " nuro >>> ["
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
				>> endl;

			gLogEvent(origin, info, LogType::DEFAULT);
		}

		void info(std::string info)
		{
			print
				>> " nuro >>> ["
				>> TextColor::GREEN
				>> "info"
				>> resetText
				>> "] "
				>> TextColor::GRAY
				>> info
				>> resetText
				>> endl;

			gLogEvent("Info", info, LogType::DEFAULT);
		}

		void info(std::string origin, std::string info)
		{
			print 
				>> " nuro >>> ["
				>> TextColor::GREEN 
				>> "info" 
				>> resetText 
				>> "] " 
				>> TextColor::GRAY 
				>> origin 
				>> ": " 
				>> resetText 
				>> info
				>> endl;

			gLogEvent(origin, info, LogType::DEFAULT);
		}

		void done(std::string origin, std::string info)
		{
			print 
				>> " nuro >>> ["
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
				>> endl;

			gLogEvent(origin, info, LogType::DEFAULT);
		}

		void debug(std::string origin, std::string info)
		{
			print
				>> TextColor::WHITE
				>> BgColor::RED
				>> " debug >>> ["
				>> "process"
				>> "] "
				>> origin
				>> ": "
				>> info
				>> "."
				>> resetText
				>> resetBg
				>> endl;
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
			print >> art >> endl;
		}

	}

}