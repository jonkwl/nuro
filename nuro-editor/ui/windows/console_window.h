#pragma once

#include "editor_window.h"

#include <string>
#include <deque>
#include <vector>

#include <utils/console.h>

using LogType = Console::out::LogType;

struct ImVec2;

struct ConsoleLog {
	std::string origin;
	std::string content;
	LogType type;

	ConsoleLog(const std::string& origin, const std::string& content, LogType type) :
		origin(origin),
		content(content),
		type(type)
	{};
};

class ConsoleWindow : public EditorWindow
{
public:
	ConsoleWindow();

	void render() override;

	// Log to console
	static void log(const ConsoleLog& log);

	// Get snapshot of current console logs
	static std::vector<ConsoleLog> snapshot();
private:
	bool showMessages;
	bool showWarnings;
	bool showErrors;

	static constexpr int32_t maxLogs = 300;
	static constexpr const char* messageIcon = ICON_FA_COMMENT;
	static constexpr const char* warningIcon = ICON_FA_TRIANGLE_EXCLAMATION;
	static constexpr const char* errorIcon = ICON_FA_CIRCLE_EXCLAMATION;
	static constexpr const char* clearIcon = ICON_FA_CROSS;

	static std::deque<ConsoleLog> logs;
	static std::vector<ConsoleLog> logsToAdd;

	// Draws a log
	void drawLog(ImDrawList& drawList, const ConsoleLog& log);

	// Draws the latest log element and returns its size
	ImVec2 drawLatestLog(ImDrawList& drawList);
};