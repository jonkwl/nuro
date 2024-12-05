#include "console_window.h"

#include <imgui.h>

#include "../src/ui/editor_ui.h"
#include "../src/ui/ui_layout.h"
#include "../src/ui/ui_components.h"
#include "../src/ui/ui_utils.h"

std::deque<ConsoleLog> ConsoleWindow::logs;
std::vector<ConsoleLog> ConsoleWindow::logsToAdd;

ConsoleWindow::ConsoleWindow() : showMessages(true),
showWarnings(true),
showErrors(true)
{
}

void ConsoleWindow::render()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Console", nullptr, EditorUI::getWindowFlags().standard);
	{
		// Get window size
		ImVec2 windowSize = ImGui::GetContentRegionAvail();

		// Draw console toolbar
		float toolbarHeight = 40.0f;
		float toolbarPaddingTop = 3.0f;
		UIComponents::space(0.0f, toolbarPaddingTop);
		UILayout::beginFlex("Toolbar", FlexType::ROW, UILayout::FULL_WIDTH, toolbarHeight, Justification::START, Alignment::CENTER, 1.0f);
		{
			UIComponents::space(10.0f, 0.0f);
			UIComponents::toggleButton(messageIcon, showMessages, "Show Messages");
			UIComponents::toggleButton(warningIcon, showWarnings, "Show Warnings");
			UIComponents::toggleButton(errorIcon, showErrors, "Show Errors");
			if (UIComponents::buttonBig("clear")) logs.clear();
		}
		UILayout::endFlex();

		// Get latest log element sizing
		float latestLogHeight = 11.0f;
		float latestLogPadding = 12.0f;
		float latestLogTotalHeight = latestLogHeight + latestLogPadding * 2;

		// Draw all log elements
		ImGui::BeginChild("Logs", ImVec2(windowSize.x, windowSize.y - toolbarHeight - latestLogTotalHeight * 2));
		{
			// Evaluate logs to add
			for (int i = 0; i < logsToAdd.size(); i++) {
				// Add new log
				logs.push_back(logsToAdd[i]);

				// Remove first log if maximum size is exceeded
				if (logs.size() > maxLogs) {
					logs.pop_front();
				}
			}

			// Draw logs
			for (int i = 0; i < logs.size(); i++) {
				drawLog(logs[i]);
			}

			// Scroll to bottom if logs were added this frame and current scroll is almost at the bottom
			if (logsToAdd.size() > 0) {
				if (UIUtils::getChildScrollValue() > 0.95f) {
					ImGui::SetScrollHereY(1.0f);
				}
			}

			// Clear logs to add
			logsToAdd.clear();
		}
		ImGui::EndChild();

		// Draw latest log element
		ImGui::SetCursorPosY(windowSize.y - latestLogTotalHeight - toolbarPaddingTop * 3 + latestLogPadding * 2);
		ImGui::BeginChild("LatestLog", ImVec2(windowSize.x, latestLogTotalHeight));
		{
			drawLatestLog(latestLogHeight, latestLogPadding);
		}
		ImGui::EndChild();
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void ConsoleWindow::addLog(ConsoleLog log)
{
	// Add log
	logsToAdd.push_back(log);
}

void ConsoleWindow::drawLog(ConsoleLog log)
{
	// Evaluate log type
	ImU32 textColor = IM_COL32(255, 255, 255, 255);
	ImU32 backgroundColor = IM_COL32(25, 25, 25, 255);
	switch (log.type) {
	case ConsoleLogType::MESSAGE:
		if (!showMessages) return;
		break;
	case ConsoleLogType::WARNING:
		if (!showWarnings) return;
		textColor = IM_COL32(255, 255, 100, 255);
		backgroundColor = IM_COL32(35, 35, 25, 255);
		break;
	case ConsoleLogType::ERROR:
		if (!showErrors) return;
		textColor = IM_COL32(255, 100, 100, 255);
		backgroundColor = IM_COL32(35, 25, 25, 255);
		break;
	}

	// Get draw list and io
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	ImGuiIO& io = ImGui::GetIO();

	// Set sizing
	float elementPadding = 18.0f;
	float originPaddingY = 6.0f;
	float height = ImGui::GetFontSize() * 2 + originPaddingY;
	float totalHeight = height + elementPadding * 2;

	// Draw background
	ImVec2 p0 = ImGui::GetCursorScreenPos();
	ImVec2 p1 = ImVec2(p0.x + ImGui::GetContentRegionAvail().x, p0.y + totalHeight);
	if (ImGui::IsMouseHoveringRect(p0, p1)) {
		backgroundColor = UIUtils::lighten(backgroundColor, 0.1f);
	}
	drawList->AddRectFilled(p0, p1, backgroundColor);

	// Draw content text with icon
	ImVec2 contentPosition = ImVec2(p0.x + elementPadding, p0.y + elementPadding);
	switch (log.type) {
	case ConsoleLogType::MESSAGE:
		drawList->AddText(contentPosition, textColor, messageIcon); break;
	case ConsoleLogType::WARNING:
		drawList->AddText(contentPosition, textColor, warningIcon); break;
	case ConsoleLogType::ERROR:
		drawList->AddText(contentPosition, textColor, errorIcon); break;
	}
	drawList->AddText(ImVec2(contentPosition.x + ImGui::GetFontSize() + originPaddingY, contentPosition.y), textColor, log.content.c_str());

	// Draw origin text
	ImVec2 originPosition = ImVec2(contentPosition.x, contentPosition.y + 20.0f);
	ImU32 originColor = IM_COL32(255, 255, 255, 180);
	drawList->AddText(originPosition, originColor, log.origin.c_str());

	// Advance cursor
	ImGui::Dummy(ImVec2(0.0f, totalHeight));
}

void ConsoleWindow::drawLatestLog(float height, float padding)
{
	// Get draw list
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	// Set sizing
	float totalHeight = height + padding * 2;

	// Draw background
	ImVec2 p0 = ImGui::GetCursorScreenPos();
	ImVec2 p1 = ImVec2(p0.x + ImGui::GetContentRegionAvail().x, p0.y + totalHeight + padding * 2);
	ImU32 backgroundColor = IM_COL32(40, 40, 40, 255);
	drawList->AddRectFilled(p0, p1, backgroundColor);

	// Evaluate text
	std::string text = "Console is empty";
	if (logs.size() > 0) {
		text = logs[logs.size() - 1].content;
	}

	// Draw text
	ImVec2 textPos = ImVec2(p0.x + padding, p0.y + padding);
	drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), text.c_str());
}