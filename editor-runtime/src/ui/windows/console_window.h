#pragma once

#include <string>
#include <deque>
#include <vector>

#include "editor_window.h"

struct ImVec2;

enum class ConsoleLogType {
	MESSAGE,
	WARNING,
	ERROR
};

struct ConsoleLog {
	std::string content;
	std::string origin;
	ConsoleLogType type;

	ConsoleLog(std::string content, std::string origin, ConsoleLogType type) :
		content(content),
		origin(origin),
		type(type)
	{};
};

class ConsoleWindow : public EditorWindow
{
public:
	ConsoleWindow();

	void render() override;

	static void addLog(const ConsoleLog& log);
private:
	bool showMessages;
	bool showWarnings;
	bool showErrors;

	static constexpr int32_t maxLogs = 300;
	static constexpr const char* messageIcon = ICON_FA_MESSAGE_LINES;
	static constexpr const char* warningIcon = ICON_FA_TRIANGLE_EXCLAMATION;
	static constexpr const char* errorIcon = ICON_FA_CIRCLE_EXCLAMATION;

	static std::deque<ConsoleLog> logs;
	static std::vector<ConsoleLog> logsToAdd;

	// Draws a log
	void drawLog(ImDrawList& drawList, const ConsoleLog& log);

	// Draws the latest log element and returns its size
	ImVec2 drawLatestLog(ImDrawList& drawList);
};