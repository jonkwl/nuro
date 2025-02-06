#include "console_window.h"

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
	ImGui::Begin(UIUtils::windowTitle("Console"), nullptr, EditorFlag::standard);
	{
		// Get 1 list
		ImDrawList& drawList = *ImGui::GetWindowDrawList();

		//
		// DRAW CONSOLE TOOLBAR
		//

		// Draw toggles
		float toolbarOffset = 34.0f;
		ImVec2 toolbarPos = ImGui::GetCursorScreenPos() + ImVec2(10.0f, 11.0f);
		IMComponents::toggleButton(drawList, messageIcon, showMessages, "Show Messages", toolbarPos);
		toolbarPos += ImVec2(toolbarOffset, 0.0f);
		IMComponents::toggleButton(drawList, warningIcon, showWarnings, "Show Warnings", toolbarPos);
		toolbarPos += ImVec2(toolbarOffset, 0.0f);
		IMComponents::toggleButton(drawList, errorIcon, showErrors, "Show Errors", toolbarPos);
		toolbarPos += ImVec2(toolbarOffset, 0.0f);
		
		// Draw clear button
		//if (IMComponents::buttonBig(ICON_FA_BRROM_WIDE " Clear")) logs.clear();
		
		// Advance cursor
		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0.0f, 50.0f));

		// 
		// DRAW LATEST LOG
		//

		ImVec2 latestLogSize = drawLatestLog(drawList);

		//
		// DRAW ALL LOGS
		//

		ImVec2 contentAvail = ImGui::GetContentRegionAvail();
		ImVec2 logsSize = ImVec2(contentAvail.x, contentAvail.y - latestLogSize.y);
		IMComponents::beginChild(logsSize);
		{
			// Evaluate logs to add
			for (int32_t i = 0; i < logsToAdd.size(); i++) {
				// Add new log
				logs.push_back(logsToAdd[i]);

				// Remove first log if maximum size is exceeded
				if (logs.size() > maxLogs) {
					logs.pop_front();
				}
			}

			// Draw logs
			for (int32_t i = 0; i < logs.size(); i++) {
				drawLog(drawList, logs[i]);
			}

			// Add spacing under last log
			ImGui::Dummy(ImVec2(0.0f, 10.0f));

			// Scroll to bottom if logs were added this frame and current scroll is almost at the bottom
			if (logsToAdd.size() > 0) {
				if (UIUtils::getChildScrollValue() > 0.95f) {
					ImGui::SetScrollHereY(1.0f);
				}
			}

			// Clear logs to add
			logsToAdd.clear();
		}
		IMComponents::endChild();
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void ConsoleWindow::addLog(const ConsoleLog& log)
{
	// Add log
	logsToAdd.push_back(log);
}

void ConsoleWindow::drawLog(ImDrawList& drawList, const ConsoleLog& log)
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

	// Set sizing
	float elementPadding = 18.0f;
	float originPaddingY = 6.0f;
	float height = ImGui::GetFontSize() * 2 + originPaddingY;
	float totalHeight = height + elementPadding * 2;

	// Draw background
	float xRectPadding = 8.0f;
	float scrollbarWidth = ImGui::GetStyle().ScrollbarSize;
	ImVec2 p0 = ImGui::GetCursorScreenPos();
	p0.x += xRectPadding;
	ImVec2 p1 = ImVec2(p0.x + ImGui::GetContentRegionAvail().x - xRectPadding - scrollbarWidth, p0.y + totalHeight);
	if (ImGui::IsMouseHoveringRect(p0, p1)) {
		backgroundColor = UIUtils::lighten(backgroundColor, 0.1f);
	}
	drawList.AddRectFilled(p0, p1, backgroundColor, 10.0f);

	// Draw content text with icon
	ImVec2 contentPosition = ImVec2(p0.x + elementPadding, p0.y + elementPadding);
	switch (log.type) {
	case ConsoleLogType::MESSAGE:
		// drawList.AddText(contentPosition, textColor, messageIcon); 
		break;
	case ConsoleLogType::WARNING:
		drawList.AddText(contentPosition, textColor, warningIcon);
		contentPosition.x += 21.0f;
		break;
	case ConsoleLogType::ERROR:
		drawList.AddText(contentPosition, textColor, errorIcon);
		contentPosition.x += 21.0f;
		break;
	}
	drawList.AddText(contentPosition, textColor, log.content.c_str());

	// Draw origin text
	ImVec2 originPosition = ImVec2(contentPosition.x, contentPosition.y + 20.0f);
	ImU32 originColor = IM_COL32(255, 255, 255, 180);
	drawList.AddText(originPosition, originColor, log.origin.c_str());

	// Advance cursor
	ImGui::Dummy(ImVec2(0.0f, totalHeight));
}

ImVec2 ConsoleWindow::drawLatestLog(ImDrawList& drawList)
{
	// Get window geometry
	ImVec2 windowPos = ImGui::GetWindowPos();
	float windowHeight = ImGui::GetWindowHeight();

	// Draw background
	ImVec2 padding = ImVec2(14.0f, 14.0f);
	ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x, EditorSizing::p_FontSize + padding.y * 2);
	ImVec2 p0 = ImVec2(windowPos.x, windowPos.y + windowHeight - size.y);
	ImVec2 p1 = p0 + size;

	// Draw background
	ImU32 backgroundColor = IM_COL32(40, 40, 40, 255);
	drawList.AddRectFilled(p0, p1, backgroundColor);

	// Evaluate text
	std::string originText = "Console empty";
	std::string contentText;
	
	if (logs.size() > 0) {
		const ConsoleLog& log = logs[logs.size() - 1];
		originText = log.origin + ":";
		contentText = log.content;
	}

	// Draw text
	ImVec2 originPosition = p0 + padding;
	ImVec2 contentPosition = originPosition + ImVec2(ImGui::CalcTextSize(originText.c_str()).x + 8.0f, 0.0f);

	drawList.AddText(EditorUI::getFonts().p_bold, EditorSizing::p_FontSize, originPosition, IM_COL32(255, 255, 255, 255), originText.c_str());
	drawList.AddText(EditorUI::getFonts().p, EditorSizing::p_FontSize, contentPosition, IM_COL32(255, 255, 255, 255), contentText.c_str());

	// Return size of element
	return size;
}