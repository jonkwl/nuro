#pragma once

#include <vector>
#include <string_view>
#include <textselect.hpp>

#include "inspectable.h"

#include "../ui/windows/console_window.h"

class WelcomeInspectable : public Inspectable
{
public:
	WelcomeInspectable();

	void renderStaticContent(ImDrawList& drawList) override;
	void renderDynamicContent(ImDrawList& drawList) override;

private:
	// Snapshot of the engines console logs at instantiation of this welcome inspectable
	std::vector<ConsoleLog> consoleSnapshot;

	// Set if game was played through welcome inspectable already
	bool playedGame;

	//
	// Text selection
	//

	std::vector<std::string_view> loggedIssues;
	std::string_view logLineAt(size_t i);
	size_t nLogs();
	TextSelect logSelect;
};